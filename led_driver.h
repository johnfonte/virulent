// Author: John Fonte
// Copyright (c) 2013
// Code to set/change RGB LED color using port C PWM pins on Teensy 2.0++

#if defined(__AVR_ATmega32U4__) || defined(__AVR_AT90USB1286__)
#define RED_ON		(PORTC |= (1<<4))
#define RED_OFF		(PORTC &= ~(1<<4))
#define BLUE_ON		(PORTC |= (1<<5))
#define BLUE_OFF	(PORTC &= ~(1<<5))
#define GREEN_ON	(PORTC |= (1<<6))
#define GREEN_OFF	(PORTC &= ~(1<<6))
#endif

#ifndef __LED_DRIVER__
#define __LED_DRIVER__

void cyclePWM(void);
void fade_through_primaries(int*, int*, int*, int*);
void breathe_color(double*, double*, double*, int*, int*, int*, int*, int*, uint16_t*, uint16_t*, uint16_t*);
int getRed(unsigned long int hex);
int getGreen(unsigned long int hex);
int getBlue(unsigned long int hex);
void setColor(unsigned long int, uint16_t*, uint16_t*, uint16_t*);
void cycle8(int*);
#endif