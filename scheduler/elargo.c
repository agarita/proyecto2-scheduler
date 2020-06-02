#include "elargo.h"

void elong_print(struct Elong * res){
  for (int i = res->largo-1; i >= 0; i--)
    printf("%d", res->numero[i]);
  printf("\n");
}

int elong_mult(int x, struct Elong * res){
  int sobrante = 0;

  for(int i = 0; i < res->largo; i++){
    int producto = res->numero[i] * x + sobrante;
    res->numero[i] = producto % 10;
    sobrante = producto / 10;
  }

  while (sobrante){
    res->numero[res->largo] = sobrante % 10;
    sobrante = sobrante / 10;
    res->largo++;
  }

  return res->largo;
}

// Una función para encontrar el factorial de un número grande
// GfG: https://www.geeksforgeeks.org/factorial-large-number/
void elong_factorial(int n, struct Elong * res){
  for(int x = 2; x <= n; x++)
    elong_mult(x, res);
}
