#include "InstructionDecoder.hpp"
#include "Exceptions.hpp"

int sign_extend(int a, int bit_pos) {
  int sign_bit = 1 << (bit_pos);
  return (a & (sign_bit - 1)) - (a & sign_bit);
}

Instruction::Instruction(int raw_instr) {
  this->raw_instr = raw_instr;
  this->opcode = raw_instr & 0x7f;
  this->inst_type = OPCODE_TO_OP_MAP.at(this->opcode); // const :(

  this->dest = 0;
  this->op1 = 0;
  this->op2 = 0;
  this->funct3 = 0;
  this->funct5 = 0;
  this->funct7 = 0;
  this->imm = 0;
  
  switch(this->inst_type) {
  case INSTR_TYPES::i:
    parseIInst(raw_instr);
    break;
  case INSTR_TYPES::u:
    parseUInst(raw_instr);
    break;
  case INSTR_TYPES::r:
    parseRInst(raw_instr);
    break;
  case INSTR_TYPES::j:
    parseJInst(raw_instr);
    break;
  case INSTR_TYPES::b:
    parseBInst(raw_instr);
    break;
  case INSTR_TYPES::s:
    parseSInst(raw_instr);
    break;
  case INSTR_TYPES::primate:
    parsePrimateInst(raw_instr);
    break;
  case INSTR_TYPES::custom:
    parseCustomInst(raw_instr);
    break;
  case INSTR_TYPES::p4:
    parseP4Inst(raw_instr);
    break;
  case INSTR_TYPES::porc:
    parsePorcInst(raw_instr);
    break;
  case INSTR_TYPES::bfu_example:
    parseBFUInst(raw_instr);
    break;
  }
}

void Instruction::parseRInst(int raw_instr) {
  dest   = (raw_instr & (0x1f      << 7))  >> 7;
  funct3 = (raw_instr & (0x7       << 12)) >> 12;
  op1    = (raw_instr & (0x1f      << 15)) >> 15;
  op2    = (raw_instr & (0x1f      << 20)) >> 20;
  funct7 = (raw_instr & (((1<<7)-1)  << 25)) >> 25;
}

void Instruction::parseIInst(int raw_instr) {
  dest =     (raw_instr & (0x1f         << 7)) >> 7;
  funct3 =   (raw_instr & (0x7          << 12)) >> 12;
  op1 =      (raw_instr & (0x1f         << 15)) >> 15;
  imm =      (raw_instr & (((1<<12)-1)    << 20)) >> 20;
  imm =      sign_extend(this->imm, 11);
}

void Instruction::parseUInst(int raw_instr) {
  dest =     (raw_instr & (0x1f         << 7)) >> 7;
  imm =      (raw_instr & (((1<<20)-1)    << 12));
}

void Instruction::parseJInst(int raw_instr) {
  dest =     (raw_instr & (0x1f         << 7)) >> 7;
  imm = 0;
  imm +=     (raw_instr & (0xf          << 21 )) >> 21 << 1;
  imm +=     (raw_instr & (1     << 20 )) >> 20 << 11;
  imm +=     (raw_instr & (((1<<8)-1)     << 12 )) >> 12 << 12;
  imm +=     (raw_instr & (1     << 31 )) >> 31 << 20;
  imm =      sign_extend(this->imm, 20);
}
void Instruction::parseBInst(int raw_instr) {
  this->imm = 0;
  this->imm +=     (raw_instr & (0xf          << 8 )) >> 8 << 1;
  this->imm +=     (raw_instr & (((1<<6)-1)     << 25 )) >> 25 << 5;
  this->imm +=     (raw_instr & (1     << 7 )) >> 7 << 11;
  this->imm +=     (raw_instr & (1     << 31 )) >> 31 << 12;

  this->funct3 =   (raw_instr & (0x7          << 12)) >> 12;
  this->op1 =      (raw_instr & (0x1f         << 15)) >> 15;
  this->op2 =      (raw_instr & (0x1f         << 20)) >> 20;
  this->imm = sign_extend(this->imm, 12);

}
void Instruction::parseSInst(int raw_instr) {
  imm =      (raw_instr & (0x1f         << 7)) >> 7;
  funct3 =   (raw_instr & (0x7          << 12)) >> 12;
  op1 =      (raw_instr & (0x1f         << 15)) >> 15;
  op2 =      (raw_instr & (0x1f         << 20)) >> 20;
  imm +=     (raw_instr & (((1<<7)-1)     << 25)) >> 25 << 5;
  imm = sign_extend(this->imm, 11);
}
void Instruction::parsePrimateInst(int raw_instr) {
  this->imm = raw_instr;
  this->is_bfu = true;
}
void Instruction::parseCustomInst(int raw_instr) {
  throw NotImplemented();
}
void Instruction::parseP4Inst(int raw_instr) {
  throw NotImplemented();
}
void Instruction::parsePorcInst(int raw_instr) {
  throw NotImplemented();
}
void Instruction::parseBFUInst(int raw_instr) {
  throw NotImplemented();
}


// // Branch unit
// void Instruction::proc_op6f(MachineState& state, MachineState& next_state){
//   Register result = state.pc + this->imm;
//   if (this->dest != 0) {
//     next_state.registerFile[this->dest] = result;
//   }
//   next_state.interconnect[this->slot_idx] = result;
//   next_state.pc = result;
// }

// // Memory Unit LOAD
// void Instruction::proc_op03(MachineState& state, MachineState& next_state){
//   Register result = state.memory[(state.registerFile[this->op1] + (this->imm))];
//   if (self.dest != 0) {
//     next_state.registerFile[this->dest] = result;
//   }
//   next_state.interconnect[this->slot_idx] = result;
// }

// // Memory Unit STORE
// void Instruction::proc_op23(MachineState& state){
//   throw NotImplemented();
// }

// // HALT 
// void Instruction::proc_op67(MachineState& state){
//   throw NotImplemented();
// }
// void Instruction::proc_op7F(MachineState& state){
//   throw NotImplemented();
// }
// void Instruction::proc_op0B(MachineState& state){
//   throw NotImplemented();
// }

std::ostream& operator<<(std::ostream& o, const Instruction& instr) {

  o << "opcode: " << std::hex << instr.opcode << std::dec << std::endl
    << "dest: " << instr.dest << std::endl
    << "ops: " << instr.op1 << ", " << instr.op2;
  return o;
}
  
