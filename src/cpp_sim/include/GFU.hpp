#pragma once

#include "util.hpp"
#include "InstructionDecoder.hpp"
#include "IExecuteUnit.hpp"

class GFU : public IExecuteUnit {
private:
  Register proc_op13(Register op1, Register op2, int32_t funct3, int32_t funct7, int32_t imm);
  Register proc_op33(Register op1, Register op2, int32_t imm, int32_t funct3, int32_t funct7);
  
public:
  bool is_branch_unit() {return false;}
  uint64_t branch_target() {return 0;}
  
  Register execute(const Register& sr1, const Register& sr2, const Instruction& instr, MachineState& ms_in, MachineState& ms_out) {

    int32_t op1 = (int32_t)(sr1);
    int32_t op2 = (int32_t)(sr2);
    Register res;
    
    ASSERT(sr1 == op1,
	   "register into GFU is too large (more than 32-bits) " << sr1
	   << " vs " << op1);
    ASSERT(sr2 == op2,
	   "register into GFU is too large (more than 32-bits)" << sr2
	   << " vs " << op2);
    switch(instr.opcode) {
    case 0x13:
      res = proc_op13(sr1, sr2, instr.funct3, instr.funct7, instr.imm);
      break;
    case 0x37:
      res = instr.imm;
      break;
    case 0x17:
      res = ms_in.pc + instr.imm;
      break;
    case 0x33:
      res = proc_op33(sr1, sr2, instr.imm, instr.funct3, instr.funct7);
      break;
    default:
      ASSERT(false,
	     "Invalid opcode into GFU opcode: " << std::hex <<
	     instr.opcode << std::dec);
    }
    if(instr.dest != 0) {
      ms_out.registerFile[instr.dest] = res;
    }
    return res;
  }
};

Register GFU::proc_op13(Register op1, Register op2, int32_t funct3, int32_t funct7, int32_t imm) {
  switch(funct3) {
  case 0:
    return op1 + imm;
  case 1:
    return op1 << imm;
  case 2:
    // signed
    return op1 < imm ? 1 : 0;
  case 3:
    // unsigned
    return op1 < imm ? 1 : 0;
  case 4:
    return op1 ^ imm;
  case 5:
    if(funct3 == 1) {
      return op1 << (int32_t) imm;
    }
    else if(funct3 == 5) {
      if(funct7 == 0) {
	return op1 >> (int32_t) imm;
      }
      else if(funct7 == 0x20) {
	return op1 >> (int32_t) imm;
      }
      else {
	assert(false && "shift bad func7");
      }
    }
    else {
      assert(false && "op33 bad funct7");
    }
  case 6:
    return op1 | imm;
  case 7:
    return op1 & imm;
  default:
    assert(false && "unknown funct in opcode 13");
  }
}

Register GFU::proc_op33(Register op1, Register op2, int32_t imm, int32_t funct3, int32_t funct7) {
  switch(funct3) {
  case 0:
    if(funct7 == 0) {
      return op1 + op2;
    }
    else if(funct7 == 32) {
      return op1 - op2;
    }
    else {
      assert(false && "op33 bad funct7");
    }
  case 1:
    return op1 << imm;
  case 2:
    return op1 < op2 ? 1 : 0;
  case 3:
    return op1 < op2 ? 1 : 0;
  case 4:
    return op1 ^ op2;
  case 5:
    if(funct3 == 1) {
      return op1 << (int32_t) op2;
    }
    else if(funct3 == 5) {
      if(funct7 == 0) {
	return op1 >> (int32_t) op2;
      }
      else if(funct7 == 0x20) {
	return op1 >> (int32_t) op2;
      }
      else {
	assert(false && "shift bad func7");
      }
    }
    else {
      assert(false && "op33 bad funct7");
    }
  case 6:
    return op1 | op2;
  case 7:
    return op1 & op2;
  default:
    assert(false && "unknown funct in opcode 13");
  }
}
