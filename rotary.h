/*
Copyright 2015 John Fonte <john@johnfonte.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ROTARY_ENCODER_H
#define  ROTARY_ENCODER_H

/* at90usb1286 MCU */
/* encoder ports */
#include <avr/io.h>

#define ENC_CTL	DDRE	//encoder port control
#define ENC_WR	PORTE	//encoder port write	
#define ENC_RD	PINE	//encoder port read
#define ENC_FLA INTF7	//encoder flag A
#define ENC_FLB INTF6	//encoder flag B
#define ENC_MSKA INT7	//encoder flag A
#define ENC_MSKB INT6	//encoder flag B
#define ENC_A 7			//encoder pin A
#define ENC_B 6			//encoder pin B
// #define ENC_VECTA INT7_vect		//encoder interrupt vector

#ifdef __cplusplus
extern "C" {
#endif

void setup_rotary_encoder(void);

#ifdef __cplusplus
}
#endif

#endif