#include "cpp_bfu.cpp"
#include <pybind11/pybind11.h>

PYBIND11_MODULE(pybind11_example, m) {
  m.doc() = "example pybind plugin";
  m.def("cpp_function", &proc, "func to print");
}
