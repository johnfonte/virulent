// Author: John Fonte
// Copyright (c) 2013
// Code to set/change RGB LED color using port C PWM pins on Teensy 2.0++

#include <avr/io.h>
#include <avr/pgmspace.h>
#include "avrpwm.h"
#include "led_driver.h"
#include <util/delay.h>
#include "usb_debug_only.h"
#include <time.h>


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

void breathe_color(double *countR, double *countG, double *countB, int *maxR, int *maxG, int *maxB, int *maxOut, int *down, uint16_t *portR, uint16_t *portG, uint16_t *portB) {
    double deltaR=((double)(*maxR)+1.0)/16.0, deltaG=((double)(*maxG)+1.0)/16.0, deltaB = ((double)(*maxB)+1.0)/16.0;   // rate of change of the led (leds are incremented 100 times a second, range is 0..max)

    // increment i and turn around if we're past an extreme
    if((*down)) {
        (*countR) -= deltaR;
        (*countG) -= deltaG;
        (*countB) -= deltaB;
        if(((*maxR) > 0 && (*countR) < 0) || ((*maxG) > 0 && (*countG) < 0) || ((*maxB) > 0 && (*countB) < 0)) {
            (*countR) = 0;
            (*countG) = 0;
            (*countB) = 0;
            (*down) = 0;
        }
    } else {
        (*countR) += deltaR;
        (*countG) += deltaG;
        (*countB) += deltaB;
        if(((*maxR) > 0 && (*countR) > (*maxR)) || ((*maxG) > 0 && (*countG) > (*maxG)) || ((*maxB) > 0 && (*countB) > (*maxB))) {
            (*countR) = (*maxR);   // would be more correct to say i=2*max-i but nobody will notice
            (*countG) = (*maxG);
            (*countB) = (*maxB);
            (*down) = 1;
        }
    }

    // set the pwm values
    *portR = (*maxOut) - (int)(*countR);
    *portG = (*maxOut) - (int)(*countG);
    *portB = (*maxOut) - (int)(*countB);
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

void setColor(unsigned long int hex, uint16_t *redPort, uint16_t *greenPort, uint16_t *bluePort) {
	unsigned long int max = 0xFF;
	*redPort = max - getRed(hex);
	*greenPort = max - getGreen(hex);
	*bluePort = max - getBlue(hex);
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

