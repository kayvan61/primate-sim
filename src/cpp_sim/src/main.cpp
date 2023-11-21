#include <iostream>
#include "consts.hpp"
#include <map>
#include <fstream>
#include <string>
#include <cstdio>
#include <boost/program_options.hpp>
#include "ProgramParser.hpp"
#include "Register.hpp"
#include "MachineState.hpp"
#include "InstructionDecoder.hpp"
#include "GFU.hpp"
#include "BranchUnit.hpp"
#include "MemoryUnit.hpp"

namespace po = boost::program_options;

const Packet& get_packet_at_pc(Program &prog, uint64_t pc) {
  for(auto& pkt: prog) {
    
  }
}

int main(int argc, const char** argv) {
  std::cout << "Start of simulation" << std::endl;
  po::options_description desc("Allowed Options");
  desc.add_options()
    ("help", "produce help message")
    ("sim_file,S", po::value<std::string>(), "file containing instruction to simulate");
  po::positional_options_description pos_opts;
  pos_opts.add("sim_file", 1);

  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv)
	    .options(desc)
	    .positional(pos_opts)
	    .run(),
	    vm);
  po::notify(vm);

  if (vm.count("help")) {
    std::cout << desc << "\n";
    return 1;
  }

  // parse program
    std::cout << "reading in file: " << vm["sim_file"].as<std::string>() << std::endl;
  std::ifstream prog_file(vm["sim_file"].as<std::string>());

  // sym table first
  std::map<std::string, uint64_t> symbol_table;
  parse_symbol_table(prog_file, symbol_table);

  // code based on sym table
  std::string line;

  std::cout << "Symbol Table:" << std::endl;
  for(auto& p: symbol_table) {
    std::cout << p.first << ": " << p.second << std::endl;
  }
  std::cout << std::endl;

  Program parsed_program;
  parse_code(prog_file, parsed_program, symbol_table);

  std::cout << "starting simulation" << std::endl;
  
  for(auto &packet: parsed_program) {
    std::cout << std::hex << packet.first << std::dec << std::endl;
    std::cout << "--------" << std::endl;
  }

  MachineState cur_state(symbol_table["main"]), next_state(symbol_table["main"]);
  const std::vector<std::vector<IExecuteUnit*>> machine_layout {
    std::vector<IExecuteUnit*>{new GFU(), new GFU(), new GFU(), new MemoryUnit(), new GFU(), new GFU()},
    std::vector<IExecuteUnit*>{new BranchUnit()}
  };
  std::vector<std::vector<Register>> interconnect(machine_layout.size());
  
  
  while(cur_state.running) {
    uint64_t cur_pc = cur_state.pc;
    const Packet& p = parsed_program[cur_pc];
    next_state.pc = cur_state.pc + PACKET_SIZE;

    auto current_machine_layer = machine_layout.begin();
    auto current_funit = current_machine_layer->begin();
    auto interconnect_in = interconnect.begin();
    auto interconnect_out = interconnect.begin();

    // exec the whole packet
    for(int i = 0; i < p.size(); i++) {
      assert(current_machine_layer != machine_layout.end() &&
	     "Packet size and lachine layout mismatch!");
      auto cur_instr = p[i];

      // layer 1 read from regfile
      Register op1 = cur_state.registerFile[cur_instr.op1];
      Register op2 = cur_state.registerFile[cur_instr.op2];
      // other layers read from interconnect
      if (current_machine_layer != machine_layout.begin()) {
	op1 = (*interconnect_in)[cur_instr.op1];
	op2 = (*interconnect_in)[cur_instr.op2];
      }

      // execute the functional unit
      Register result = (*current_funit)->execute(op1, op2, cur_instr, cur_state, next_state);
      // if dest is not 0 then write
      if(cur_instr.dest != 0) {
	next_state.registerFile[cur_instr.dest] = result;
      }
      (*interconnect_out).push_back(result);

      // advance to next funit
      current_funit++;

      // next layer
      if(current_funit == current_machine_layer->end()) {
	current_machine_layer++;
	current_funit = current_machine_layer->begin();
	if(interconnect_in != interconnect_out) {
	  interconnect_in++;
	}
	interconnect_out++;
      }
    }
    assert(current_machine_layer == machine_layout.end() &&
	   "Packet size and lachine layout mismatch!");
    int c = getchar();
    
    cur_state = next_state;
  }
  
  return 0;
}
