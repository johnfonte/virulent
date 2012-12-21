// Author: John Fonte
// Copyright (c) 2013
// Code to set/change RGB LED color using port C PWM pins on Teensy 2.0++

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "avrpwm.h"
#include "led_driver.h"
#include <util/delay.h>
#include "usb_debug_only.h"


void cyclePWM(void) {
	unsigned char countR;//, countG, countB;
	DDRC |= (1<<4);
	for(countR = 0; countR < 0xFF; countR++) {
		OCR3C = 0xFF - countR;
		OCR3B = 0xFF;
		OCR3A = 0xFF;
		// RED_ON;
		_delay_ms(10);
	}
}

void fade_through_primaries(int *current, int *max, int *state, int *down) {
	int numStates = 7;
	int delta = (*max)/0xF;
	if((*down)) {
		(*current) -= delta;
		if((*current) < 0) {
			(*current) = -(*current);
			(*down) = 0;
			(*state)++;
			if((*state) >= numStates) (*state) = 0;
		}
	} else {
		(*current) += delta;
		if((*current) > (*max)) {
			(*current) = (*max);
			(*down) = 1;
		}
	}
	switch((*state)) {
		case 0: {
			OCR3C = (*max) - (*current);
			OCR3B = (*max);
			OCR3A = (*max);
			break;
		} case 1: {
			OCR3C = (*max);
			OCR3B = (*max) - (*current);
			OCR3A = (*max);
			break;
		} case 2: {
			OCR3C = (*max);
			OCR3B = (*max);
			OCR3A = (*max) - (*current);
			break;
		} case 3: {
			OCR3C = (*max) - (*current);
			OCR3B = (*max) - (*current);
			OCR3A = (*max);
			break;
		} case 4: {
			OCR3C = (*max) - (*current);
			OCR3B = (*max);
			OCR3A = (*max) - (*current);
			break;
		} case 5: {
			OCR3C = (*max);
			OCR3B = (*max) - (*current);
			OCR3A = (*max) - (*current);
			break;
		} case 6: {
			OCR3C = (*max) - (*current);
			OCR3B = (*max) - (*current);
			OCR3A = (*max) - (*current);
			break;
		} case 7: {
			OCR3C = (*max);
			OCR3B = (*max);
			OCR3A = (*max);
			break;
		}
	}
	_delay_ms(50);
}

void breathe_color(double *i, double *j, double *k, int *red, int *green, int *blue, int *max, int *down) {
    double idelta=((double)(*red)+1.0)/16.0, jdelta=((double)(*green)+1.0)/16.0, kdelta = ((double)(*blue)+1.0)/16.0;   // rate of change of the led (leds are incremented 100 times a second, range is 0..max)

    // increment i and turn around if we're past an extreme
    if((*down)) {
        (*i) -= idelta;
        (*j) -= jdelta;
        (*k) -= kdelta;
        if(((*red) > 0 && (*i) < 0) || ((*green) > 0 && (*j) < 0) || ((*blue) > 0 && (*k) < 0)) {
            (*i) = 0;
            (*j) = 0;
            (*k) = 0;
            (*down) = 0;
        }
    } else {
        (*i) += idelta;
        (*j) += jdelta;
        (*k) += kdelta;
        if(((*red) > 0 && (*i) > (*red)) || ((*green) > 0 && (*j) > (*green)) || ((*blue) > 0 && (*k) > (*blue))) {
            (*i) = (*red);   // would be more correct to say i=2*max-i but nobody will notice
            (*j) = (*green);
            (*k) = (*blue);
            (*down) = 1;
        }
    }

    // set the pwm values
    OCR3C = (*max) - (int)(*i);
    OCR3B = (*max) - (int)(*j);
    OCR3A = (*max) - (int)(*k);
    _delay_ms(50);
}

int getRed(unsigned long int hex) {
	return(((hex >> 16) & 0xFF) << 0);
}

int getGreen(unsigned long int hex) {
	return(((hex >> 8) & 0xFF) << 0);
}

int getBlue(unsigned long int hex) {
	return((hex & 0xFF) << 0);
}

void setColor(unsigned long int hex) {
	unsigned long int max = 0xFF;
	OCR3C = max - getRed(hex);
	OCR3B = max - getGreen(hex);
	OCR3A = max - getBlue(hex);
}

void cycle8(int *count) {
	for((*count) = 0; (*count) < 0x8; (*count) ++) {
		switch((*count)) {
			case 0: {
				RED_OFF;
				BLUE_OFF;
				GREEN_OFF;
				break;
			} case 1: {
				RED_ON;
				BLUE_OFF;
				GREEN_OFF;
				break;
			} case 2: {
				RED_OFF;
				BLUE_ON;
				GREEN_OFF;
				break;
			} case 3: {
				RED_OFF;
				BLUE_OFF;
				GREEN_ON;
				break;
			} case 4: {
				RED_ON;
				BLUE_ON;
				GREEN_OFF;
				break;
			} case 5: {
				RED_ON;
				BLUE_OFF;
				GREEN_ON;
				break;
			} case 6: {
				RED_OFF;
				BLUE_ON;
				GREEN_ON;
				break;
			} case 7: {
				RED_ON;
				BLUE_ON;
				GREEN_ON;
				break;
			} default: {
				RED_OFF;
				BLUE_OFF;
				GREEN_OFF;
				break;
			}
		}
	_delay_ms(250);
	}
}

