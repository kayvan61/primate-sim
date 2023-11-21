#pragma once

#include "Register.hpp"
#include <cstdint>
#include <vector>
#include <map>


class MachineState{
public:
  std::vector<Register> registerFile;
  std::map<uint64_t, Register> memory;

  uint32_t pc;
  bool running;

  MachineState(uint64_t starting_addr);
};
