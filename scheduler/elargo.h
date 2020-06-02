#ifndef elargo_H_
#define elargo_H_

#include <stdio.h>

#define MAX_F 35660

struct Elong{
  int numero[MAX_F];
  int largo;
};

void elong_print(struct Elong * res);

int elong_mult(int x, struct Elong * res);

void elong_factorial(int n, struct Elong * res);

#endif
