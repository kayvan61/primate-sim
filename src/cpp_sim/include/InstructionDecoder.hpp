#pragma once

#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <vector>

#include "MachineState.hpp"

enum INSTR_TYPES {
 i = 0,
 u,
 r,
 j,
 b,
 s,
 primate,
 custom,
 p4,
 porc,
 bfu_example
};

const std::map<int, INSTR_TYPES> OPCODE_TO_OP_MAP = {
  {    0b0010011, i },
  {    0b0110111, u },
  {    0b0010111, u },
  {    0b0110011, r },
  {    0b1101111, j },
  {    0b1100111, j },
  {    0b1100011, b },
  {    0b0000011, i },
  {    0b0100011, s },
  {    0b0001111, i },
  {    0b1110011, i },
  {    0b0001011, primate },
  {    0b0011011, custom },
  {    0b1001011, custom },
  {    0b0101011, custom },
  {    0b1011011, p4 },
  {    0b1111011, porc},
  {    0b1111111, bfu_example}
};

class Instruction {
public:
  int raw_instr;

  INSTR_TYPES inst_type;

  bool is_bfu;

  int opcode;
  int dest;
  int op1, op2;
  int funct3, funct5, funct7;
  int imm;
  
  Instruction(int raw_instr);

  friend std::ostream& operator<<(std::ostream& o, const Instruction& instr);

private:
  void parseIInst(int raw_instr);
  void parseUInst(int raw_instr);
  void parseRInst(int raw_instr);
  void parseJInst(int raw_instr);
  void parseBInst(int raw_instr);
  void parseSInst(int raw_instr);
  void parsePrimateInst(int raw_instr);
  void parseCustomInst(int raw_instr);
  void parseP4Inst(int raw_instr);
  void parsePorcInst(int raw_instr);
  void parseBFUInst(int raw_instr);
};

using Packet = std::vector<Instruction>;
using Program = std::map<uint64_t, Packet>;
