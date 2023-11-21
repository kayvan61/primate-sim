from cffi import FFI
import math
import sys
import dataclasses

class Register:
    def __init__(self, raw, c_ptr):
        self.raw_bytes = raw
        self.c_struct  = c_ptr

class BFU_Interface:
    def __init__(self, bfu_file, header_string):
        # start creation of c interface
        ffi = FFI()

        ffi.cdef(header_string)

        ffi.set_source("_bfu",
                       header_string,
                       sources = [bfu_file],
                       )
        ffi.compile(verbose=False)
        
        from _bfu import lib
        from _bfu import ffi as bfu_ffi
        
        self.ffi = bfu_ffi
        self.lib = lib
        
    def pyth_to_c_raw_bytes(self, in_obj, size_in_bytes):
        return self.ffi.new(f'char[{size_in_bytes}]')

    def c_to_pyth_raw_bytes(self, obj, size_in_bytes):
        raw_bytes = []
        for i in range(size_in_bytes):
            raw_bytes.append(obj[i])
        return raw_bytes

    def pyth_to_c_big_num(self, num):
        num_bytes = math.ceil((num.bit_length()+1)/8)
        bytes_ptr = self.ffi.new(f"char[{num_bytes}]")
        print(bytes_ptr)
        for i, val in enumerate(num.to_bytes(num_bytes, 'little', signed=True)):
            assert val.bit_length() <= 8, "to_bytes iterator gave too large a number"
            bytes_ptr[i] = bytes([val])
        return bytes_ptr, num_bytes

    def c_to_pyth_big_num(self, obj):
        print(obj)
        buf = bytes(self.ffi.buffer(obj))
        return int.from_bytes(buf, byteorder='little', signed=True)

    def pyth_to_c_reg(self, num):
        ptr, byte_len = self.pyth_to_c_big_num(num)
        reg = self.ffi.new('Register *', [ptr, byte_len]);
        return Register(ptr, reg)

    def c_to_pyth_reg(self, reg):
        return self.c_to_pyth_big_num(reg.value)

import pybind11_example
    
if __name__ == "__main__":

    answer = pybind11_example.cpp_function(15)
    print(f"python got {answer} back from cpp")
    
    print("========= WARNING =========")
    print("self test not implemented")
    

