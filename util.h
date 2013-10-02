// Author: John Fonte
// Copyright (c) 2013
// Template for utility functions related to RGB LEDs on Teensy 2.0++

#ifndef __UTIL__
#define __UTIL__

typedef enum { false, true } bool;

double add(double, double);

double sub(double, double);

bool less(double, double);

bool more(double, double);

int getRed(unsigned long int);

int getGreen(unsigned long int);

int getBlue(unsigned long int);
#endif
