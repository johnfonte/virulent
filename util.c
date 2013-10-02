#include <avr/io.h>
#include <avr/pgmspace.h>
#include "util.h"

double add(double cnt, double delt) {
  return cnt + delt;
}

double sub(double cnt, double delt) {
  return cnt - delt;
}

bool less(double cnt, double min) {
  return (cnt < min);
}

bool more(double cnt, double max) {
  return (cnt > max);
}

int getRed(unsigned long int hex) {
  return((hex >> 16) & 0xFF);
}

int getGreen(unsigned long int hex) {
  return((hex >> 8) & 0xFF);
}

int getBlue(unsigned long int hex) {
  return(hex & 0xFF);
}
