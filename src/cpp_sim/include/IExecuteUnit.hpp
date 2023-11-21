#pragma once

#include "InstructionDecoder.hpp"
#include "MachineState.hpp"

class IExecuteUnit {
public:
  virtual Register execute(const Register&, const Register&, const Instruction&, MachineState&, MachineState&) = 0;
  virtual bool is_branch_unit() = 0;
  virtual uint64_t branch_target() = 0;
};
