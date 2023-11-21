#include "Register.hpp"
#include <cstdint>
#include <cmath>

// Register::Register(int bitsize) {
//   this->num_elements = std::ceil(bitsize / 32.0);
//   raw_ints = new uint32_t[this->num_elements];
// }

// Register::Register(const Register&) {
  
// }

// Register& Register::operator=(const Register&) {
//   return *this;
// }

// Register::~Register() {
//   delete raw_bytes;
// }

// Register Register::operator+(int32_t a) {
//   int num_eles = max(this->num_elements, 4);
//   Register res(num_eles);
//   bool carry_in = false;
//   for (int i = 0; i < num_eles; i++) {
//     // op
//     uint64_t op1 = this->raw_ints[i];
//     uint64_t op2 = 0;
//     if(i == 0) 
//       op2 = a;
//     uint64_t result = op1 + op2;
//     if(carry_in)
//       result++;
      
//     // carry
//     if(result != result & 0x0FFFFFFFF) {
//       // generate a carry
//       carry_in = true;
//     } else {
//       carry_in = false;
//     }
//     res->raw_ints[i] = result & 0x0FFFFFFFF;
//   }

//   return res;
// }

// Register Register::operator+(const Register& o) {
//   int num_eles = max(this->num_elements, o.num_elements);
//   Register res(num_eles);
//   bool carry_in = false;
//   for (int i = 0; i < num_eles; i++) {
//     // op
//     uint64_t op1 = i < this->num_elements ? this->raw_ints[i] : 0;
//     uint64_t op2 = i < o.num_elements ? o.raw_ints[i] : 0;
//     uint64_t result = op1 + op2;
//     if(carry_in)
//       result++;
      
//     // carry
//     if(result != result & 0x0FFFFFFFF) {
//       // generate a carry
//       carry_in = true;
//     } else {
//       carry_in = false;
//     }
//     res->raw_ints[i] = result & 0x0FFFFFFFF;
//   }

//   return res;
// }

// Register Register::operator-(int32_t) {

// }

// Regsiter Register::operator-(const Register& o) {
//   int num_eles = max(this->num_elements, o.num_elements);
//   Register res(num_eles);
//   bool carry_in = false;
//   for (int i = 0; i < num_eles; i++) {
//     // op
//     uint64_t op1 = i < this->num_elements ? this->raw_ints[i] : 0;
//     uint64_t op2 = i < o.num_elements ? o.raw_ints[i] : 0;
//     uint64_t result = op1 - op2;
//     if(carry_in)
//       result++;
      
//     // carry
//     if(result != result & 0x0FFFFFFFF) {
//       // generate a carry
//       carry_in = true;
//     } else {
//       carry_in = false;
//     }
//     res->raw_ints[i] = result & 0x0FFFFFFFF;
//   }

//   return res;
// }
// Register Register::operator<<(int32_t);
// Register Register::operator<<(Register);
// Register Register::operator^(int32_t);
// Register Register::operator^(Register);
// Register Register::operator|(int32_t);
// Register Register::operator|(Register);
// Register Register::operator&(int32_t);
// Register Register::operator&(Register);
