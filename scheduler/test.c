
/*
double arcsin (int n){
  double sum = 0.0;
  addend = 1.0;

  for(int i = 0; i < n; i++){
    addend = (2*i)/(4);
    sum = addend;
  }

  return sum;
}
*/
//An aproximation with the Talor Series of 4arctan(1). It's a lot less intensive than 2arcsin(1), but it's not what we're looking for.
//Negating a number: https://stackoverflow.com/questions/15111466/fastest-way-to-negate-a-number
//4arctan(1) in C: https://stackoverflow.com/questions/43702289/calculating-pi-using-the-taylor-series-c
double arctan (int n){
  double sum = 0.0;
  int sign = 1;

  for (int i = 0; i < n; i++){
    sum += sign / (2.0*i+1.0);
    sign = -sign;
  }

  return 4*sum;
}
