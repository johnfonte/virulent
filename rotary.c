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

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "usb_keyboard.h"
#include "rotary.h"

void setup_rotary_encoder(void) {
  ENC_CTL &= ~(_BV(ENC_A)|_BV(ENC_B)); //inputs
  ENC_WR |= (_BV(ENC_A)|_BV(ENC_B));    //turn on pullups
  EIFR |= (_BV(ENC_FLA)|_BV(ENC_FLB));  //enable encoder pins interrupt sources
  // setting encoder flags
 
  /* enable pin change interrupts */
  // setting mask for allowed interrupts
  EIMSK |= (_BV(ENC_MSKA)|_BV(ENC_MSKB));
}

void rotary_encoder(void) {
  static uint8_t old_AB = ENC_DETENT_STATE;  //lookup table index
  uint8_t encport;
  uint8_t direction;
  static const int8_t enc_states [] PROGMEM = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};  //encoder lookup table

  old_AB <<=2;  //remember previous state
  encport = ( ( ENC_RD & 0x30 ) >> 4);
  old_AB |= encport;
  direction = pgm_read_byte(&(enc_states[( old_AB & 0x0f )]));
  if(encport == ENC_DETENT_STATE) {
    if(direction == 1) {
      rot_key_press(KEY_A);
    } else if(direction == -1) {
      rot_key_press(KEY_S);
    }
  }

}

ISR(INT5_vect) {
  rotary_encoder();
}

ISR(INT4_vect) {
  rotary_encoder();
}
