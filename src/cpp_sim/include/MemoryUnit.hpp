#pragma once

#include "util.hpp"
#include "InstructionDecoder.hpp"
#include "IExecuteUnit.hpp"

class MemoryUnit : public IExecuteUnit {
private:
  
public:
  bool is_branch_unit() {return false;}
  uint64_t branch_target() {return 0;}
  
  Register execute(const Register& sr1, const Register& sr2, const Instruction& instr, MachineState& ms_in, MachineState& ms_out) {

    int32_t op1 = (int32_t)(sr1);
    int32_t op2 = (int32_t)(sr2);
    int32_t addr = op1 + instr.imm;
    
    ASSERT(sr1 == op1,
	   "register into MemoryUnit is too large (more than 32-bits) " << sr1
	   << " vs " << op1);
    
    switch(instr.opcode) {
    case 0x13:
      return 0;
    case 0x03:
      // load
      return ms_in.memory[addr];
    case 0x23:
      // store
      ms_out.memory[addr] = sr2;
      return 0;
    default:
      ASSERT(false,
	     "Invalid opcode into MemoryUnit opcode: " << std::hex <<
	     instr.opcode << std::dec);
    }
  }
};
