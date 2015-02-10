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
  ENC_WR |= (_BV(ENC_A)|_BV(ENC_B));    //turn on pullups
  EIFR |= (_BV(ENC_FLA)|_BV(ENC_FLB));  //enable encoder pins interrupt sources
  // setting encoder flags
 
  /* enable pin change interrupts */
  // setting mask for allowed interrupts
  EIMSK |= (_BV(ENC_MSKA)|_BV(ENC_MSKB));
}

volatile int8_t encval = 0;   //encoder value
volatile uint8_t direction = 0; // indicates encoder direction

ISR(INT7_vect) {
	static uint8_t old_AB = 0;  //lookup table index
  static const int8_t enc_states [] PROGMEM = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};  //encoder lookup table

  old_AB <<=2;  //remember previous state
  old_AB |= ( ENC_RD & 0x03 );
  direction = pgm_read_byte(&(enc_states[( old_AB & 0x0f )]));
  if(direction > 0) {
    // vol up
    usb_keyboard_press(KEY_W, 0);
  } else if(direction < 0) {
    // vol down
    usb_keyboard_press(KEY_S, 0);
  }
  encval += direction;
  /* post "Navigation forward/reverse" event */
  // if( encval > 3 ) {  //four steps forward
  //   QF::publish(Q_NEW(QEvent, NAV_FWD_SIG));
  //   encval = 0;
  // }
  // else if( encval < -3 ) {  //four steps backwards
  //   QF::publish(Q_NEW(QEvent, NAV_REV_SIG));
  //   encval = 0;
  // }
}