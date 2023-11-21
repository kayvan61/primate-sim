BFU_SRC_DIR=/work/amansoor/primate-sim/src/bfu_tests

export PYTHONPATH=`realpath ./build`:${PYTHONPATH}

mkdir build || true
g++ -O3 -Wall -Werror -shared -std=c++11 -fPIC ${BFU_SRC_DIR}/cpp_bfu.cpp -o ./build/libbfutest.so
g++ -O3 -Wall -Werror -shared -std=c++11 -fPIC `python3 -m pybind11 --includes` -I /usr/include/python3.7 -I . -I ./build ${BFU_SRC_DIR}/pybind.cpp -o ./build/pybind11_example`python3.10-config --extension-suffix` -L. -L./build -lbfutest -Wl,-rpath,.

python3 ./src/sim/utils.py 
