#pragma once

#include "InstructionDecoder.hpp"
#include "IExecuteUnit.hpp"
#include "consts.hpp"

class BranchUnit : public IExecuteUnit {
  uint64_t target;
public:
  Register execute(const Register& sr1, const Register& sr2, const Instruction& instr, MachineState& ms_in, MachineState& ms_out) {

    assert((sr1 & 0x0ffffffff) == sr1 && "Branch unit got ops larger than 32-bits");
    assert((sr2 & 0x0ffffffff) == sr2 && "Branch unit got ops larger than 32-bits");
    int dest = instr.dest;
    Register res;
    switch(instr.opcode) {
    case 0x13: // nop
      return 0;
    case 0x6f:
      target = ms_in.pc + instr.imm;
      res = Register(ms_in.pc + PACKET_SIZE);
      break;
    case 0x67:
      target = (uint32_t)(sr1 + instr.imm);
      res = (ms_in.pc + PACKET_SIZE);
      break;
    case 0x63:
      dest = 0;
      switch(instr.funct3) {
      case 0:
	target = ms_in.pc + (sr1 == sr2 ? instr.imm : PACKET_SIZE);
	break;
      case 1:
	target = ms_in.pc + (sr1 != sr2 ? instr.imm : PACKET_SIZE);
	break;
      case 4:
	target = ms_in.pc + (sr1 < sr2 ? instr.imm : PACKET_SIZE);
	break;
      case 6:
	target = ms_in.pc + (sr1 < sr2 ? instr.imm : PACKET_SIZE); // unsigned
	break;
      case 5:
	target = ms_in.pc + (sr1 >= sr2 ? instr.imm : PACKET_SIZE);
	break;
      case 7:
	target = ms_in.pc + (sr1 >= sr2 ? instr.imm : PACKET_SIZE); // unsigned
	break;
      default:
	assert(false && "bad funct3 into branch unit");
      }
      break;
    default:
      assert(false && "bad opcode into branch unit");
    };

    
    ms_out.pc = target;
    if (dest != 0) {
      ms_out.registerFile[dest] = res;
    }
    
    return res;
  }

  uint64_t branch_target() {
    return target;
  }

  bool is_branch_unit() {
    return true;
  }
};
