#include "MachineState.hpp"
#include "consts.hpp"

MachineState::MachineState(uint64_t starting_addr) : 
  registerFile(NUM_REGISTERS, 0), 
  memory(), 
  pc(starting_addr),
  running(true)
{

}
