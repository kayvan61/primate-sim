#pragma once

#include <cstdint>
#include <boost/multiprecision/cpp_int.hpp>

using Register = boost::multiprecision::cpp_int;

// class Register {
//   uint32_t  num_elements;
//   uint32_t* raw_ints;
// public:
//   Register(int bitsize);
//   Register(const Register&);
//   Register& operator=(const Register&);
//   ~Register();

//   Register operator+(const int32_t);
//   Register operator+(const Register&);
//   Register operator-(const int32_t);
//   Regsiter operator-(const Register&);
//   Register operator<<(const int32_t);
//   Register operator<<(const Register&);
//   Register operator^(const int32_t);
//   Register operator^(const Register&);
//   Register operator|(const int32_t);
//   Register operator|(const Register&);
//   Register operator&(const int32_t);
//   Register operator&(const Register&);
  
//   bool operator<(const Register&);
//   bool operator<(const int32_t);
//   bool operator>=(const Register&);

//   bool unsigned_lt(const int32_t);
// };
