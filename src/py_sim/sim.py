import os
import argparse
import random
import copy
from ctypes import *
from instr import Instruction
from dataclasses import dataclass, field
from global_params import GLOBAL_PARAMS
from cffi import FFI

bfu_file = "./bfu_tests/printing_bfu.c"

# start creation of c interface
ffi = FFI()
header_string = """
    typedef struct {
        char **regfile;
        char **memory;
        char **interconnect;
    } MachineState;

    void proc(MachineState* sr1, MachineState* sr2); // list all the function prototypes
"""

ffi.cdef(header_string)

ffi.set_source("_bfu",
    header_string,
    sources = [bfu_file],
)
ffi.compile(verbose=False)

from _bfu import lib
from _bfu import ffi as bfu_ffi

#
# GLOBALS 
#

# R-type : funct7, rs2, rs1, funct3, rd,  opcode
# I-type : imm[11:0], rs1, funct3, rd, opcode
# S-type : imm[11:5], rs2, rs1, funct3, imm[4:0], opcode
# B-type : imm[12], imm[10:5], rs2, rs1, funct3, imm[4:1], imm[11], opcode
# U-type : imm[31:12], rd, opcode
# J-type : imm[20], imm[10:1], imm[11], imm[19:12], rdopcode

DATA_SIZE = 4

RISCV_REG_NAMES = [
    ('x0', 'zero'),
    ('x1', 'ra'),
    ('x2', 'sp'),
    ('x3', 'gp'),
    ('x4', 'tp'),
    ('x5', 't0'),
    ('x6', 't1'),
    ('x7', 't2'),
    ('x8', 's0/fp'),
    ('x9', 's1'),
    ('x10', 'a0'),
    ('x11', 'a1'),
    ('x12', 'a2'),
    ('x13', 'a3'),
    ('x14', 'a4'),
    ('x15', 'a5'),
    ('x16', 'a6'),
    ('x17', 'a7'),
    ('x18', 's2'),
    ('x19', 's3'),
    ('x20', 's4'),
    ('x21', 's5'),
    ('x22', 's6'),
    ('x23', 's7'),
    ('x24', 's8'),
    ('x25', 's9'),
    ('x26', 's10'),
    ('x27', 's11'),
    ('x28', 't3'),
    ('x29', 't4'),
    ('x30', 't5'),
    ('x31', 't6'),
]


DEFAULT_MACHINE_LAYOUT = [6,1]

@dataclass
class MachineParams:
    REG_WIDTH: int = 192
    LAYER_SLOT_COUNT: list = field(default_factory=lambda: DEFAULT_MACHINE_LAYOUT)
    MEMORY_SIZE: int = int((1 << 16) / REG_WIDTH) ## 64k of mem

class MachineState:
    def __init__(self, pc: int, input_reg: bytes, params: MachineParams):
        self.regs = [0] * 32
        self.pc = pc
        self.memory = [random.randint(0, 1<<32)] * params.MEMORY_SIZE
        self.interconnect = [0] * sum(params.LAYER_SLOT_COUNT)
        self.halted = False
        self.input_reg = input_reg
        self.params = params
        self.printed_done = False


SYMBOL_TABLE = {}
PROGRAM_TEXT = {}
DRAM_CONTENTS = {}

def seek_to_line(file_handle, seek_to, strip=True):
    for line in file_handle:
        if strip:
            line = line.strip()
        if line.strip() == seek_to:
            break

def read_file_into_ram(f_path : str):
    print("opening", f_path, "to simulate")
    with open(f_path) as f:
        ## Hunt for symbol table
        seek_to_line(f, "SYMBOL TABLE:")
        
        ## sym table found
        for line in f:
            if len(line.strip()) > 0:
                toks = line.strip().split()
                if toks[-3] == ".text":
                    SYMBOL_TABLE[toks[-1]] = int(toks[0], 16)
            else:
                break

        print("Symbol Table found:")
        print(SYMBOL_TABLE)

        ## Find .text section
        seek_to_line(f, "Disassembly of section .text:")

        lines_read = 0
        instructions = 0
        instr_in_current_packet = 7
        skipped_instrs = 0
        for line in f:
            line = line.strip()
            if len(line) <= 0:
                continue
            lines_read += 1
            toks = [x.strip() for x in line.split(":") if len(x.strip()) > 0]
            if GLOBAL_PARAMS.debug:
                print(line)
            if len(toks) == 1:
                ## found a section
                if toks[0] == "--------":
                    if instr_in_current_packet != 7:
                        print("OH HECK")
                        skipped_instrs += 1
                    instr_in_current_packet = 0
                    continue
                print("entering section", toks[0])
            elif len(toks) == 2:
                ## found an instruction
                PROGRAM_TEXT[int(toks[0],16)] = toks[1].split('\t')[0]
                instructions += 1
                instr_in_current_packet += 1
            else:
                print("unhandled parsing case. tokens found:", toks, "line:", line)
                exit(-1)

        if lines_read == 0:
            print(".text section is empty")
            exit(-1)
        else:
            print("found .text. read", lines_read, "lines")
            print("             read", instructions, "instructions")
            print("   missing instrs", skipped_instrs, "instructions")
            print()

def hex_str_to_dec(string, little_endian=True):
    str_bytes = string.split()
    if not little_endian:
        str_bytes = str_bytes[::-1]
    
    ret = sum( int(x, 16) << i*8 for i,x in enumerate(str_bytes) )
    return ret

def text_to_dram():
    for addr, asm in PROGRAM_TEXT.items():
        DRAM_CONTENTS[addr] = Instruction(hex_str_to_dec(asm))

def print_regs(regs):
    print("="*10)
    for name_tup, value in zip(RISCV_REG_NAMES, regs):
        name, common_name = name_tup
        if value != 0:
            print(name, common_name, value, sep='\t')
    print("="*10)

def simulate_sequential(entry_point, interactive, packets):
    try:
        pc = SYMBOL_TABLE[entry_point]
    except:
        print(f"Entry point {entry_point} not found in symbol table")
        exit(-1)
    print("starting simulation at PC:", pc, f"({entry_point})")

    machineparams = MachineParams()

    ###### start simulation ######

    # just spawn all the threads at start...
    threads = []
    for packet in packets:
        threads.append(MachineState(pc, packet, machineparams)) 

    # run each thread in sequence
    # still run packet by packet though
    print("done with thread init")
    while any([not x.halted for x in threads]):
        for thread_num in range(len(threads)):
            thread = threads[thread_num]

            # run the entire thread
            while not thread.halted:
                packet_pcs = [thread.pc + i*4 for i in range(7)]
                cur_insts = [DRAM_CONTENTS[x] for x in packet_pcs]
                next_state = copy.deepcopy(thread)

                for slot_idx, instr in enumerate(cur_insts):
                    instr.proc(thread, next_state)
                threads[thread_num] = next_state
                thread = next_state


def simulate(entry_point, interactive, packets):
    try:
        pc = SYMBOL_TABLE[entry_point]
    except:
        print(f"Entry point {entry_point} not found in symbol table")
        exit(-1)
    print("starting simulation at PC:", pc, f"({entry_point})")

    machineparams = MachineParams()

    ###### start simulation ######

    # just spawn all the threads at start...
    threads = []
    for packet in packets:
        threads.append(MachineState(pc, packet, machineparams)) 


    # while any thread is not halted
    while any([not x.halted for x in threads]):
        if GLOBAL_PARAMS.debug:
            print([hex(x.pc) for x in threads])
        for thread_num in range(len(threads)):
            thread = threads[thread_num]

            if thread.halted:
                if not thread.printed_done:
                    print("thread", thread_num, "is done")
                    thread.printed_done = True
                continue

            # gather a packet
            packet_pcs = [thread.pc + i*4 for i in range(7)]
            cur_insts = [DRAM_CONTENTS[x] for x in packet_pcs]
            jumpped = False
            next_state = copy.deepcopy(thread)

            # advance the PC. If it is a taken branch this is overriden when executing
            next_state.pc = thread.pc + 4*7 

            # exec the packet
            for slot_idx, instr in enumerate(cur_insts):
                instr.exec(thread, next_state, slot_idx)

            if GLOBAL_PARAMS.debug:
                if thread_num == 0:
                    print("interconnect:", thread.interconnect)
                    print("registers:", thread.regs)

            threads[thread_num] = next_state
    
    # for thread in threads:
    #     print_regs(thread.regs)
        
def read_input_file(input_fname: str):
    packets = []
    with open(input_fname) as f:
        current_packet = []
        for line in f: 
            is_last, unused, data = line.split(' ')
            unused = int(unused)
            is_last = int(is_last)
            data = data.strip()
            if len(data) % 2 != 0:
                data = '0'+data
            data = bytes.fromhex(data.strip()) # throws on odd number of characters
            if unused == 0:
                current_packet.extend(data)
            else:
                current_packet.extend(data[:(-unused * 2)])
            if is_last == 1:
                packets.append(current_packet)
                current_packet = [];
    return packets


def bfu_link(bfu_fname: str):
    Instruction.bfu_lib = lib
    Instruction.bfu_ffi = bfu_ffi

def main(args):
    bfu_link(bfu_file)

    read_file_into_ram(args.program)
    text_to_dram()

    input_data = read_input_file(args.input)
    print("All packets are same sized: ", all([len(x) == 192 for x in input_data]))
    print("Read", len(input_data), "packets")
    print()

    if args.diff_mode:
        seq_results = simulate_sequential(args.entry, false, input_data)
        para_results = simulate(args.entry, false, input_data)
    else:  
        simulate(args.entry, args.interactive, input_data)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
                    prog='PrimateSim',
                    description='ISA simulator for Primate')
    parser.add_argument("-p", "--program", required=True, help="Which program to simulate")
    parser.add_argument("-i", "--input", required=True, help="File that contains input data in the format spec'd in the doc")

    parser.add_argument("-e", "--entry", default="main", help="Which section to start the simulation. defaults to 'main'")
    parser.add_argument("--interactive", default=False, help="start in interactive mode", action='store_true')
    parser.add_argument("--debug", default=False, help="enable debug mode", action="store_true")
    parser.add_argument("-d", "--diff_mode", help="secify a riscv file to run against as the golden truth model")
    args = parser.parse_args()

    GLOBAL_PARAMS.debug = args.debug
    main(args)
