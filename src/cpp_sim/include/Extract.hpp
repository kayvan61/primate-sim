#pragma once

#include "Instruction.hpp"
#include "IExecuteUnit.hpp"

class Extract: public IExecuteUnit {
public:
  Register execute(Instruction& instr, MachineState& ms_in) {
    Register res(32); // TODO: PARAM THE DATAPATH WITH
    switch(instr.opcode) {
    case 0x2B:
      switch(instr.funct3) {
      case 0x0:
	return ; // extract
      case 0x1:
	return ; // insert 
      default:
	assert(false && "Invalid funct3 into extract/insert unit");
      };
    default:
      assert(false && "Invalid opcode into extract/insert unit");
    }
  }
};
