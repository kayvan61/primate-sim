
import random # random init of memory
from itertools import zip_longest
from global_params import GLOBAL_PARAMS

# Opcode -> (RISC type, function map)
OPCODE_TO_OP_MAP = {
    0b0010011: ("i"), 
    0b0110111: ("u"), 
    0b0010111: ("u"), 
    0b0110011: ("r"), 
    0b1101111: ("j"), 
    0b1100111: ("j"), 
    0b1100011: ("b"), 
    0b0000011: ("i"), 
    0b0100011: ("s"), 
    0b0001111: ("i"), 
    0b1110011: ("i"), 
    0b0001011: ("primate"), 
    0b0011011: ("custom"), 
    0b1001011: ("custom"), 
    0b0101011: ("custom"), 
    0b1011011: ("p4"), 
    0b1111011: ("porc"),
    0b1111111: ("bfu_example")
}

def sign_extend(value, bits):
    sign_bit = 1 << (bits)
    return (value & (sign_bit - 1)) - (value & sign_bit)

class Instruction:
    OPCODE_MAP = 0x07f
    called_once = False
    bfu_lib = None
    bfu_ffi = None
    def __init__(self, instruction):
        if (Instruction.bfu_lib is None or Instruction.bfu_ffi is None) and not Instruction.called_once:
            print("Warning! bfu functions not linked in!")
            print("\tthis is fine if there are no BFU's in your program.")
            Instruction.called_once = True

        self.instruction = instruction
        self.opcode = instruction & Instruction.OPCODE_MAP
        self.is_bfu = False
        try:
            self.type = OPCODE_TO_OP_MAP[self.opcode]
        except:
            print("unknown opcode type. Instruction", hex(instruction))
            exit(-1)

        {
            "r": self.parse_r_type,
            "i": self.parse_i_type,
            "s": self.parse_s_type,
            "b": self.parse_b_type,
            "u": self.parse_u_type,
            "j": self.parse_j_type,
            "bfu_example": self.parse_bfu_type,
            "primate": self.parse_bfu_type
        }[self.type](instruction)

        try:
            self.proc = {
                0x13: self.proc_op13,
                0x37: self.proc_op37,
                0x17: self.proc_op17,
                0x33: self.proc_op33,
                0x6f: self.proc_op6f,
                0x03: self.proc_op03,
                0x23: self.proc_op23,
                0x67: self.proc_op67,
                0x63: self.proc_op63,
                0x7F: self.proc_bfu,
                0x0B: self.proc_input_done
            }[self.opcode]
        except Exception as e:
            print()
            print(type(e), e)
            print("Error in binding opcode! Opcode:", hex(self.opcode))
            print("Instruction:", hex(self.instruction))
            exit(-1)

    def parse_bfu_type(self, instruction):
        self.imm = instruction;
        self.is_bfu = True

    def parse_r_type(self, instruction):
        self.dest   = (instruction & (0x1f      << 7))  >> 7
        self.funct3 = (instruction & (0x7       << 12)) >> 12
        self.op1    = (instruction & (0x1f      << 15)) >> 15
        self.op2    = (instruction & (0x1f      << 20)) >> 20
        self.funct7 = (instruction & (((1<<7)-1)  << 25)) >> 25

    def parse_i_type(self, instruction):
        self.dest =     (instruction & (0x1f         << 7)) >> 7
        self.funct3 =   (instruction & (0x7          << 12)) >> 12
        self.op1 =      (instruction & (0x1f         << 15)) >> 15
        self.imm =      (instruction & (((1<<12)-1)    << 20)) >> 20
        self.imm = sign_extend(self.imm, 11)
        
    def parse_s_type(self, instruction):
        self.imm =      (instruction & (0x1f         << 7)) >> 7
        self.funct3 =   (instruction & (0x7          << 12)) >> 12
        self.op1 =      (instruction & (0x1f         << 15)) >> 15
        self.op2 =      (instruction & (0x1f         << 20)) >> 20
        self.imm +=     (instruction & (((1<<7)-1)     << 25)) >> 25 << 5
        self.imm = sign_extend(self.imm, 11)

    def parse_b_type(self, instruction):
        self.imm = 0
        self.imm +=     (instruction & (0xf          << 8 )) >> 8 << 1
        self.imm +=     (instruction & (((1<<6)-1)     << 25 )) >> 25 << 5
        self.imm +=     (instruction & (1     << 7 )) >> 7 << 11
        self.imm +=     (instruction & (1     << 31 )) >> 31 << 12

        self.funct3 =   (instruction & (0x7          << 12)) >> 12
        self.op1 =      (instruction & (0x1f         << 15)) >> 15
        self.op2 =      (instruction & (0x1f         << 20)) >> 20
        self.imm = sign_extend(self.imm, 12)

    def parse_u_type(self, instruction):
        self.dest =     (instruction & (0x1f         << 7)) >> 7
        self.imm =      (instruction & (((1<<20)-1)    << 12))

    def parse_j_type(self, instruction):
        self.dest =     (instruction & (0x1f         << 7)) >> 7

        self.imm = 0
        self.imm +=     (instruction & (0xf          << 21 )) >> 21 << 1
        self.imm +=     (instruction & (1     << 20 )) >> 20 << 11
        self.imm +=     (instruction & (((1<<8)-1)     << 12 )) >> 12 << 12
        self.imm +=     (instruction & (1     << 31 )) >> 31 << 20
        self.imm = sign_extend(self.imm, 20)

    def exec(self, state, next_state, slot_idx):
        self.slot_idx = slot_idx
        return self.proc(state, next_state)

    def shift_op(self):
        if (self.imm & (1<<10)) == 1:
            result = (state.regs[self.op1]) >> (self.imm & 0x1f)
        else:
            result = (state.regs[self.op1] & ((1 << state.params.REG_WIDTH)-1)) >> (self.imm & 0x1f)
        return result 

    # read the packet into the registers
    def proc_input_done(self, state, next_state):
        in_reg = state.input_reg
        reg_in_bytes = state.params.REG_WIDTH / 8
        chunker = [iter(in_reg)] * int(reg_in_bytes)
        for reg_idx, chunk in enumerate(zip_longest(*chunker)):
            next_state.regs[reg_idx+1] = int.from_bytes(chunk, 'little')

    def proc_bfu(self, state, next_state):
        print("temp")
        my_object = Instruction.bfu_ffi.new('MachineState*')
        my_object.regfile = [x.to_bytes(int(state.params.REG_WIDTH / 8), 'little', signed=True) for x in state.regs]
        my_object.memory = [x.to_bytes(int(state.params.REG_WIDTH / 8), 'little', signed=True) for x in state.memory]
        my_object.interconnect = [x.to_bytes(int(state.params.REG_WIDTH / 8), 'little', signed=True) for x in state.interconnect]
        ret = Instruction.bfu_lib.proc(my_object, my_object)
        print("got", ret, "from the bfu!")

    # i type
    def proc_op13(self, state, next_state):
        match(self.funct3):
            case 0:
                result = state.regs[self.op1] + self.imm
            case 1:
                result = state.regs[self.op1] << self.imm
            case 2:
                result = 1 if state.regs[self.op1] < self.imm else 0
            case 3:
                result = 1 if (state.regs[self.op1] & ((1 << state.params.REG_WIDTH)-1)) < (self.imm & ((1 << state.params.REG_WIDTH)-1)) else 0
            case 4:
                result = (state.regs[self.op1] & ((1 << state.params.REG_WIDTH)-1)) ^ (self.imm & ((1 << state.params.REG_WIDTH)-1))
            case 5:
                result = self.shift_op()
            case 6:
                result = (state.regs[self.op1] & ((1 << state.params.REG_WIDTH)-1)) | (self.imm & ((1 << state.params.REG_WIDTH)-1))
            case 7:
                result = (state.regs[self.op1] & ((1 << state.params.REG_WIDTH)-1)) & (self.imm & ((1 << state.params.REG_WIDTH)-1))
            case _:
                print("unknown funct in opcode 13")
                exit(-1)
        if self.dest != 0:
            next_state.regs[self.dest] = result 
        state.interconnect[self.slot_idx] = result
        return result 

    # u type
    def proc_op37(self, state, next_state):
        result = self.imm
        if self.dest != 0:
            next_state.regs[self.dest] = result 
        state.interconnect[self.slot_idx] = result
        return result 

    # u type
    def proc_op17(self, state, next_state):
        result = state.pc + self.imm
        if self.dest != 0:
            next_state.regs[self.dest] = result 
        state.interconnect[self.slot_idx] = result
        return result 
    
    def add_reg_reg(self, state):
        if self.funct7 == 0:
            result = state.regs[self.op1] + state.regs[self.op2]
        elif self.funct7 == 32:
            result = state.regs[self.op1] - state.regs[self.op2]
        state.interconnect[self.slot_idx] = result
        return result

    # r-type
    def proc_op33(self, state, next_state):
        match(self.funct3):
            case 0:
                result = self.add_reg_reg(state)
            case 1:
                result = state.regs[self.op1] << self.imm
            case 2:
                result = 1 if state.regs[self.op1] < state.regs[self.op2] else 0
            case 3:
                result = 1 if (state.regs[self.op1] & ((1 << state.params.REG_WIDTH)-1)) < (state.regs[self.op2] & ((1 << state.params.REG_WIDTH)-1)) else 0
            case 4:
                result = (state.regs[self.op1] & ((1 << state.params.REG_WIDTH)-1)) ^ (state.regs[self.op2] & ((1 << state.params.REG_WIDTH)-1))
            case 5:
                result = self.shift_op()
            case 6:
                result = (state.regs[self.op1] & ((1 << state.params.REG_WIDTH)-1)) | (state.regs[self.op2] & ((1 << state.params.REG_WIDTH)-1))
            case 7:
                result = (state.regs[self.op1] & ((1 << state.params.REG_WIDTH)-1)) & (state.regs[self.op2] & ((1 << state.params.REG_WIDTH)-1))
            case _:
                print("unknown funct in opcode 13")
                exit(-1)
        if self.dest != 0:
            next_state.regs[self.dest] = result 
        state.interconnect[self.slot_idx] = result
        return result 
    
    # TODO: Branch targets...
    def proc_op6f(self, state, next_state):
        result = state.pc
        if self.dest != 0:
            next_state.regs[self.dest] = result 
        state.interconnect[self.slot_idx] = result
        return result 
    
    # TODO: Word addressable memory only...
    def proc_op03(self, state, next_state):
        result = state.memory[(state.regs[self.op1] + (self.imm))]
        if self.dest != 0:
            next_state.regs[self.dest] = result 
        state.interconnect[self.slot_idx] = result
        return result 
    
    def proc_op23(self, state, next_state):
        next_state.memory[(state.regs[self.op1] + (self.imm))] = state.regs[self.op2] 
    
    def proc_op67(self, state, next_state):
        next_state.halted = True

    def proc_op63(self, state, next_state):
        if GLOBAL_PARAMS.debug:
            print("branch instr!")
            print(f"imm: {self.imm}")
        taken = False
        target = state.pc
        match(self.funct3):
            case 0:
                # r1 == r2
                if GLOBAL_PARAMS.debug:
                    print(f"{state.interconnect[self.op1]} == {state.interconnect[self.op2]}")
                taken = (state.interconnect[self.op1]) == (state.interconnect[self.op2])
                target = state.pc + self.imm
            case 1:
                # r1 != r2
                if GLOBAL_PARAMS.debug:
                    print(f"{state.interconnect[self.op1]} != {state.interconnect[self.op2]}")
                taken = (state.interconnect[self.op1]) != (state.interconnect[self.op2])
                target = state.pc + self.imm
            case 4:
                # r1 < r2 signed
                if GLOBAL_PARAMS.debug:
                    print(f"{state.interconnect[self.op1]} < {state.interconnect[self.op2]}")
                taken = (state.interconnect[self.op1]) < (state.interconnect[self.op2])
                target = state.pc + self.imm
            case 5:
                # r1 >= r2 signed
                if GLOBAL_PARAMS.debug:
                    print(f"{state.interconnect[self.op1]} >= {state.interconnect[self.op2]}")
                taken = (state.interconnect[self.op1]) >= (state.interconnect[self.op2])
                target = state.pc + self.imm
            case 6:
                # r1 < r2 unsigned
                if GLOBAL_PARAMS.debug:
                    print(f"{state.interconnect[self.op1]} < {state.interconnect[self.op2]} us")
                taken = (state.interconnect[self.op1] & ((1 << state.params.REG_WIDTH)-1)) < (state.interconnect[self.op2] & ((1 << state.params.REG_WIDTH)-1))
                target = state.pc + self.imm
            case 7:
                # r1 >= r2 unsigned
                if GLOBAL_PARAMS.debug:
                    print(f"{state.interconnect[self.op1]} >= {state.interconnect[self.op2]} us")
                taken = (state.interconnect[self.op1] & ((1 << state.params.REG_WIDTH)-1)) >= (state.interconnect[self.op2] & ((1 << state.params.REG_WIDTH)-1))
                target = state.pc + self.imm
            case _:
                print("unknown funct in opcode 63")
                exit(-1)
        if taken:
            # TODO: hack for branch pc offsets. 
            if GLOBAL_PARAMS.debug:
                print("taken branch! target:", hex(target + 6*4))
            next_state.pc = target + 6*4

