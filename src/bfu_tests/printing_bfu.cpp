#include <iostream>
#include <stdio.h>

typedef struct __PythNum {
  char *value;
  int length;
} PythNum;

typedef struct {
    PythNum *regfile;
    PythNum *memory;
    PythNum *interconnect;
} MachineState;

void proc(MachineState* sr1, MachineState* sr2);

void proc1(PythNum* sr1);

void proc(MachineState* sr1, MachineState* sr2) {
  printf("%x", sr1->regfile[0].value[0]);
}

void proc1(PythNum* sr1) {
  sr1->value[0]++;
  printf("%x", sr1->value[0]);
}
