#pragma once

#include <map>
#include <boost/algorithm/string/classification.hpp>     // Include boost::for is_any_of
#include <boost/algorithm/string/split.hpp>              // Include for boost::split
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <map>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include "InstructionDecoder.hpp"

void seek_to_line(std::ifstream& file, const std::string& target) {
  std::string line;
  while(std::getline(file, line)) {
    boost::trim(line);
    if(!line.compare(target)) {
      return;
    }
  }
}

void parse_symbol_table(std::ifstream& prog_file, std::map<std::string, uint64_t>& res_map) {
  seek_to_line(prog_file, "SYMBOL TABLE:");
  std::string line;
  while(std::getline(prog_file, line)) {
    // break to parse code
    if(!line.compare("Disassembly of section .text:")) {
      std::cout << "Found the .text section" << std::endl;
      break;
    }
    
    // tokenize the string
    std::vector<std::string> words;
    boost::split(words, line, boost::is_any_of(" \t"), boost::token_compress_on);
    if(words.size() == 6)
      res_map[words[5]] = std::stoul(words[0], nullptr, 16);
  }
}

void parse_code(std::ifstream& prog_file, Program& parsed_program, std::map<std::string, uint64_t>& symbol_table) {
  uint64_t packet_pc = 0;
  Packet cur_packet;
  std::string line;
  const boost::regex section_header_regex{"[0-9]+ <[a-z]+>:"};
  const boost::regex section_name_regex{"<[a-z]+>"};
  while(std::getline(prog_file, line)) {
    boost::trim(line);
    
    if(line.length() == 0) {
      continue;
    }

    // token out the line
    std::vector<std::string> tokens;
    boost::split(tokens, line, boost::is_any_of(":"), boost::token_compress_on);
    tokens.erase(std::remove_if(tokens.begin(),
				tokens.end(),
				[](std::string& a) {return a.length() == 0;}),
		 tokens.end());
    std::for_each(tokens.begin(),
		  tokens.end(),
		  [](std::string& str){ boost::trim(str); });
    if (tokens.size() == 1) {
      if(!line.compare("--------") && cur_packet.size() > 0) {
	std::cout << packet_pc << std::endl;
	parsed_program[packet_pc] = cur_packet;
	cur_packet = Packet();
	packet_pc += PACKET_SIZE;
	continue;
      }
      else if (boost::regex_match(line, section_header_regex)) {
	boost::sregex_iterator it(line.begin(), line.end(), section_name_regex);
	boost::sregex_iterator end;
	if(std::distance(it, end) > 1) {
	  assert(false && "Multiple section names found on section header (regex iterator distance > 1)");
	}
	std::string section_name = it->str();
	section_name.erase(section_name.begin());
	section_name.erase(section_name.end()-1);
	packet_pc = symbol_table.at(section_name);
	std::cout << "starting parsing on section: " << section_name
		  << " at PC: " << std::hex << packet_pc << std::dec << std::endl;
      }
    }
    else if (tokens.size() == 2) {
      std::vector<std::string> instr_tokens;
      boost::split(instr_tokens, tokens[1], boost::is_any_of("\t "), boost::token_compress_on);
      int raw_instr_bytes = 0;
      for(auto it = instr_tokens.begin()+3; it >= instr_tokens.begin(); it--) {
	raw_instr_bytes <<= 8;
	raw_instr_bytes += std::stoul(*it, nullptr, 16) & 0xff;
      }
      cur_packet.push_back(Instruction(raw_instr_bytes));
    }
    else {
      std::cout << "a 3rd case!" << std::endl;
      std::cout << line << std::endl;
    }
  }
}
