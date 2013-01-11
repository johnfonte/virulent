/* USB Keyboard Firmware code for the Virulent Keyboard
 * Using the Teensy 2.0++ Microcontroller
 * Adapted from Phantom Keyboard Firmware
 * Copyright (c) 2013 John Fonte
 *
 * Copyright (c) 2012 Fredrik Atmer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notices and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usb_keyboard.h"
#include "led_driver.h"
#include "avrpwm.h"
#include "usb_debug_only.h"

#define CPU_PRESCALE(n) (CLKPR = 0x80, CLKPR = (n))

#define bool            uint8_t
#define true            1
#define false           0
#define NULL            0
#define NA              0

#define _DDRB           (uint8_t *const)&DDRB
#define _DDRC           (uint8_t *const)&DDRC
#define _DDRD           (uint8_t *const)&DDRD
#define _DDRE           (uint8_t *const)&DDRE
#define _DDRF           (uint8_t *const)&DDRF

#define _PINB           (uint8_t *const)&PINB
#define _PINC           (uint8_t *const)&PINC
#define _PIND           (uint8_t *const)&PIND
#define _PINE           (uint8_t *const)&PINE
#define _PINF           (uint8_t *const)&PINF

#define _PORTB          (uint8_t *const)&PORTB
#define _PORTC          (uint8_t *const)&PORTC
#define _PORTD          (uint8_t *const)&PORTD
#define _PORTE          (uint8_t *const)&PORTE
#define _PORTF          (uint8_t *const)&PORTF

#define _OCR1A          (uint8_t *const)&OCR1A
#define _OCR1B          (uint8_t *const)&OCR1B
#define _OCR1C          (uint8_t *const)&OCR1C
#define _OCR3A          (uint8_t *const)&OCR3A
#define _OCR3B          (uint8_t *const)&OCR3B
#define _OCR3C          (uint8_t *const)&OCR3C

#define _PIN0 0x01
#define _PIN1 0x02
#define _PIN2 0x04
#define _PIN3 0x08
#define _PIN4 0x10
#define _PIN5 0x20
#define _PIN6 0x40
#define _PIN7 0x80

/* NROW number of rows
   NCOL number of columns
   NKEY = NROW*NCOL */
#define NROW            5
#define NCOL            19
#define NKEY            95
#define MODES           4
#define INDICATE        6
#define MAINPINS        3

#define WHITE    0xFFFFFF
#define RED      0xFF0000
#define GREEN    0x00FF00
#define BLUE     0x0000FF
#define YELLOW   0xFFFF00
#define PURPLE   0xFF00FF
#define CYAN     0x00FFFF

/* Modifier keys are handled differently and need to be identified */
const uint8_t is_modifier[MODES][NKEY] = {
{ // LAYOUT 0
  NA,              NA,              NA,              NA,              NA,     // COL  0
  false,           false,           false,           false,           NA,     // COL  1
  false,           false,           false,           false,           NA,     // COL  2
  false,           false,           false,           false,           NA,     // COL  3
  false,           false,           false,           false,           NA,     // COL  4

  true,            false,           false,           false,           false,  // COL  5
  true,            true,            false,           false,           false,  // COL  6
  true,            false,           false,           false,           false,  // COL  7
  true,            false,           false,           false,           false,  // COL  8
  NA,              false,           false,           false,           false,  // COL  9
  true,            false,           false,           false,           false,  // COL 10
  false,           false,           false,           false,           false,  // COL 11
  false,           false,           false,           false,           false,  // COL 12
  true,            false,           false,           false,           false,  // COL 13
  NA,              false,           false,           false,           false,  // COL 14
  true,            false,           false,           false,           false,  // COL 15
  false,           false,           false,           false,           false,  // COL 16
  false,           false,           NA,              false,           false,  // COL 17
  false,           true,            false,           false,           false,  // COL 18
}, { // LAYOUT 1
  NA,              NA,              NA,              NA,              NA,     // COL  0
  false,           false,           false,           false,           false,  // COL  1
  false,           false,           false,           false,           false,  // COL  2
  false,           false,           false,           false,           false,  // COL  3
  false,           false,           false,           false,           false,  // COL  4

  true,            false,           false,           false,           false,  // COL  5
  true,            true,            false,           false,           false,  // COL  6
  true,            false,           false,           false,           false,  // COL  7
  true,            false,           false,           false,           false,  // COL  8
  NA,              false,           false,           false,           false,  // COL  9
  true,            false,           false,           false,           false,  // COL 10
  false,           false,           false,           false,           false,  // COL 11
  false,           false,           false,           false,           false,  // COL 12
  true,            false,           false,           false,           false,  // COL 13
  NA,              false,           false,           false,           false,  // COL 14
  true,            false,           false,           false,           false,  // COL 15
  false,           false,           false,           false,           false,  // COL 16
  false,           false,           NA,              false,           false,  // COL 17
  false,           true,            false,           false,           false,  // COL 18
}, { // LAYOUT 2
  NA,              NA,              NA,              NA,              NA,     // COL  0
  false,           false,           false,           NA,              NA,     // COL  1
  false,           false,           false,           NA,              false,  // COL  2
  false,           false,           false,           false,           false,  // COL  3
  false,           false,           false,           NA,              false,  // COL  4

  true,            false,           false,           false,           false,  // COL  5
  true,            true,            false,           false,           false,  // COL  6
  true,            false,           false,           false,           false,  // COL  7
  true,            false,           false,           false,           false,  // COL  8
  NA,              false,           false,           false,           false,  // COL  9
  true,            false,           false,           false,           false,  // COL 10
  false,           false,           false,           false,           false,  // COL 11
  false,           false,           false,           false,           false,  // COL 12
  true,            false,           false,           false,           false,  // COL 13
  NA,              false,           false,           false,           false,  // COL 14
  true,            false,           false,           false,           false,  // COL 15
  false,           false,           false,           false,           false,  // COL 16
  false,           false,           NA,              false,           false,  // COL 17
  false,           true,            false,           false,           false,  // COL 18
}, { // LAYOUT 3
  NA,              false,           false,           false,           false,  // COL  0
  false,           false,           false,           false,           false,  // COL  1
  false,           false,           false,           false,           false,  // COL  2
  true,            false,           false,           false,           false,  // COL  3
  true,            false,           false,           false,           false,  // COL  4

  true,            false,           false,           false,           false,  // COL  5
  true,            true,            false,           false,           false,  // COL  6
  true,            false,           false,           false,           false,  // COL  7
  true,            false,           false,           false,           false,  // COL  8
  NA,              false,           false,           false,           false,  // COL  9
  true,            false,           false,           false,           false,  // COL 10
  false,           false,           false,           false,           false,  // COL 11
  false,           false,           false,           false,           false,  // COL 12
  true,            false,           false,           false,           false,  // COL 13
  NA,              false,           false,           false,           false,  // COL 14
  true,            false,           false,           false,           false,  // COL 15
  false,           false,           false,           false,           false,  // COL 16
  false,           false,           NA,              false,           false,  // COL 17
  false,           true,            false,           false,           false,  // COL 18
} };

const uint8_t layout[MODES][NKEY] = {
{ // LAYOUT 0
//ROW 0            ROW 1            ROW 2            ROW 3            ROW 4
  NA,              NA,              NA,              NA,              NA,                 // COL  0
  KEY_LEFT,        KEY_F9,          KEY_F5,          KEY_F1,          NA,                 // COL  1
  KEY_UP,          KEY_F10,         KEY_F6,          KEY_F2,          NA,                 // COL  2
  KEY_DOWN,        KEY_F11,         KEY_F7,          KEY_F3,          NA,                 // COL  3
  KEY_RIGHT,       KEY_F12,         KEY_F8,          KEY_F4,          NA,                 // COL  4

  KEY_LEFT_CTRL,   KEY_APPLICATION, KEY_ESC,         KEY_TAB,         KEY_TILDE,          // COL  5
  KEY_LEFT_ALT,    KEY_LEFT_SHIFT,  KEY_A,           KEY_Q,           KEY_1,              // COL  6
  KEY_LEFT_GUI,    KEY_Z,           KEY_S,           KEY_W,           KEY_2,              // COL  7
  KEY_LEFT_SHIFT,  KEY_X,           KEY_D,           KEY_E,           KEY_3,              // COL  8
  NA,              KEY_C,           KEY_F,           KEY_R,           KEY_4,              // COL  9
  KEY_LEFT_CTRL,   KEY_V,           KEY_G,           KEY_T,           KEY_5,              // COL 10
  KEY_BACKSPACE,   KEY_B,           KEY_H,           KEY_Y,           KEY_6,              // COL 11
  KEY_SPACE,       KEY_N,           KEY_J,           KEY_U,           KEY_7,              // COL 12
  NA,              KEY_M,           KEY_K,           KEY_I,           KEY_8,              // COL 13
  KEY_RIGHT_ALT,   KEY_COMMA,       KEY_L,           KEY_O,           KEY_9,              // COL 14
  KEY_RIGHT_GUI,   KEY_PERIOD,      KEY_SEMICOLON,   KEY_P,           KEY_0,              // COL 15
  KEY_LEFT,        KEY_SLASH,       KEY_QUOTE,       KEY_LEFT_BRACE,  KEY_MINUS,          // COL 16
  KEY_DOWN,        KEY_UP,          NA,              KEY_RIGHT_BRACE, KEY_EQUAL,          // COL 17
  KEY_RIGHT,       KEY_RIGHT_SHIFT, KEY_ENTER,       KEY_BACKSLASH,   KEY_BACKSPACE,      // COL 18
}, { // LAYOUT 1
//ROW 0            ROW 1            ROW 2            ROW 3            ROW 4
  NA,              NA,              NA,              NA,              NA,                 // COL  0
  KEY_LEFT,        KEY_0,           KEY_1,           KEY_4,           KEY_7,              // COL  1
  KEY_UP,          KEY_PERIOD,      KEY_2,           KEY_5,           KEY_8,              // COL  2
  KEY_DOWN,        KEYPAD_PLUS,     KEY_3,           KEY_6,           KEY_9,              // COL  3
  KEY_RIGHT,       KEYPAD_ENTER,    KEYPAD_MINUS,    KEYPAD_SLASH,    KEYPAD_ASTERIX,     // COL  4

  KEY_LEFT_CTRL,   KEY_APPLICATION, KEY_ESC,         KEY_TAB,         KEY_TILDE,          // COL  5
  KEY_LEFT_ALT,    KEY_LEFT_SHIFT,  KEY_A,           KEY_Q,           KEY_1,              // COL  6
  KEY_LEFT_GUI,    KEY_Z,           KEY_S,           KEY_W,           KEY_2,              // COL  7
  KEY_LEFT_SHIFT,  KEY_X,           KEY_D,           KEY_E,           KEY_3,              // COL  8
  NA,              KEY_C,           KEY_F,           KEY_R,           KEY_4,              // COL  9
  KEY_LEFT_CTRL,   KEY_V,           KEY_G,           KEY_T,           KEY_5,              // COL 10
  KEY_BACKSPACE,   KEY_B,           KEY_H,           KEY_Y,           KEY_6,              // COL 11
  KEY_SPACE,       KEY_N,           KEY_J,           KEY_U,           KEY_7,              // COL 12
  NA,              KEY_M,           KEY_K,           KEY_I,           KEY_8,              // COL 13
  KEY_RIGHT_ALT,   KEY_COMMA,       KEY_L,           KEY_O,           KEY_9,              // COL 14
  KEY_RIGHT_GUI,   KEY_PERIOD,      KEY_SEMICOLON,   KEY_P,           KEY_0,              // COL 15
  KEY_LEFT,        KEY_SLASH,       KEY_QUOTE,       KEY_LEFT_BRACE,  KEY_MINUS,          // COL 16
  KEY_DOWN,        KEY_UP,          NA,              KEY_RIGHT_BRACE, KEY_EQUAL,          // COL 17
  KEY_RIGHT,       KEY_RIGHT_SHIFT, KEY_ENTER,       KEY_BACKSLASH,   KEY_BACKSPACE,      // COL 18
}, { // LAYOUT 2
//ROW 0            ROW 1            ROW 2            ROW 3            ROW 4
  NA,              NA,              NA,              NA,              NA,                 // COL  0
  KEY_LEFT,        KEY_STOP,        KEY_F5,          NA,              NA,                 // COL  1
  KEY_UP,          KEY_END,         KEY_HOME,        NA,              KEY_MUTE,           // COL  2
  KEY_DOWN,        KEY_PAGE_DOWN,   KEY_PAGE_UP,     KEY_PAUSE,       KEY_VOLUMEDOWN,     // COL  3
  KEY_RIGHT,       KEY_DELETE,      KEY_INSERT,      NA,              KEY_VOLUMEUP,       // COL  4

  KEY_LEFT_CTRL,   KEY_APPLICATION, KEY_ESC,         KEY_TAB,         KEY_TILDE,          // COL  5
  KEY_LEFT_ALT,    KEY_LEFT_SHIFT,  KEY_A,           KEY_Q,           KEY_1,              // COL  6
  KEY_LEFT_GUI,    KEY_Z,           KEY_S,           KEY_W,           KEY_2,              // COL  7
  KEY_LEFT_SHIFT,  KEY_X,           KEY_D,           KEY_E,           KEY_3,              // COL  8
  NA,              KEY_C,           KEY_F,           KEY_R,           KEY_4,              // COL  9
  KEY_LEFT_CTRL,   KEY_V,           KEY_G,           KEY_T,           KEY_5,              // COL 10
  KEY_BACKSPACE,   KEY_B,           KEY_H,           KEY_Y,           KEY_6,              // COL 11
  KEY_SPACE,       KEY_N,           KEY_J,           KEY_U,           KEY_7,              // COL 12
  NA,              KEY_M,           KEY_K,           KEY_I,           KEY_8,              // COL 13
  KEY_RIGHT_ALT,   KEY_COMMA,       KEY_L,           KEY_O,           KEY_9,              // COL 14
  KEY_RIGHT_GUI,   KEY_PERIOD,      KEY_SEMICOLON,   KEY_P,           KEY_0,              // COL 15
  KEY_LEFT,        KEY_SLASH,       KEY_QUOTE,       KEY_LEFT_BRACE,  KEY_MINUS,          // COL 16
  KEY_DOWN,        KEY_UP,          NA,              KEY_RIGHT_BRACE, KEY_EQUAL,          // COL 17
  KEY_RIGHT,       KEY_RIGHT_SHIFT, KEY_ENTER,       KEY_BACKSLASH,   KEY_BACKSPACE,      // COL 18
}, { // LAYOUT 3
//ROW 0            ROW 1            ROW 2            ROW 3            ROW 4
  NA,              KEY_Z,           KEY_A,           KEY_Q,           KEY_1,              // COL  0
  KEY_F10,         KEY_X,           KEY_S,           KEY_W,           KEY_2,              // COL  1
  KEY_F11,         KEY_C,           KEY_D,           KEY_E,           KEY_3,              // COL  2
  KEY_LEFT_SHIFT,  KEY_V,           KEY_F,           KEY_R,           KEY_4,              // COL  3
  KEY_LEFT_CTRL,   KEY_B,           KEY_G,           KEY_T,           KEY_5,              // COL  4

  KEY_LEFT_CTRL,   KEY_APPLICATION, KEY_ESC,         KEY_TAB,         KEY_TILDE,          // COL  5
  KEY_LEFT_ALT,    KEY_LEFT_SHIFT,  KEY_A,           KEY_Q,           KEY_1,              // COL  6
  KEY_LEFT_GUI,    KEY_Z,           KEY_S,           KEY_W,           KEY_2,              // COL  7
  KEY_LEFT_SHIFT,  KEY_X,           KEY_D,           KEY_E,           KEY_3,              // COL  8
  NA,              KEY_C,           KEY_F,           KEY_R,           KEY_4,              // COL  9
  KEY_LEFT_CTRL,   KEY_V,           KEY_G,           KEY_T,           KEY_5,              // COL 10
  KEY_BACKSPACE,   KEY_B,           KEY_H,           KEY_Y,           KEY_6,              // COL 11
  KEY_SPACE,       KEY_N,           KEY_J,           KEY_U,           KEY_7,              // COL 12
  NA,              KEY_M,           KEY_K,           KEY_I,           KEY_8,              // COL 13
  KEY_RIGHT_ALT,   KEY_COMMA,       KEY_L,           KEY_O,           KEY_9,              // COL 14
  KEY_RIGHT_GUI,   KEY_PERIOD,      KEY_SEMICOLON,   KEY_P,           KEY_0,              // COL 15
  KEY_LEFT,        KEY_SLASH,       KEY_QUOTE,       KEY_LEFT_BRACE,  KEY_MINUS,          // COL 16
  KEY_DOWN,        KEY_UP,          NA,              KEY_RIGHT_BRACE, KEY_EQUAL,          // COL 17
  KEY_RIGHT,       KEY_RIGHT_SHIFT, KEY_ENTER,       KEY_BACKSLASH,   KEY_BACKSPACE,      // COL 18
} };

/* Specifies the ports and pin numbers for the rows */
uint8_t *const  row_ddr[NROW] = { _DDRB,  _DDRB,  _DDRB,  _DDRB,  _DDRB};
uint8_t *const row_pull[NROW] = {_PORTB, _PORTB, _PORTB, _PORTB, _PORTB};
uint8_t *const row_port[NROW] = { _PINB,  _PINB,  _PINB,  _PINB,  _PINB};
const uint8_t   row_bit[NROW] = { _PIN0,  _PIN1,  _PIN2,  _PIN3,  _PIN4};

/* Specifies the ports and pin numbers for the indicators lights only */
uint8_t *const  ind_ddr[INDICATE] = { _DDRC,  _DDRC,  _DDRC,  _DDRC,  _DDRC,  _DDRC};
uint8_t *const ind_port[INDICATE] = {_PORTC, _PORTC, _PORTC, _PORTC, _PORTC, _PORTC};
const uint8_t   ind_bit[INDICATE] = { _PIN0,  _PIN1,  _PIN2,  _PIN4,  _PIN5,  _PIN6};

/* Specifies the ports and pin numbers for the indicators lights only */
uint8_t *const  main_ddr[MAINPINS] = { _DDRB,  _DDRB,  _DDRB};
uint8_t *const main_port[MAINPINS] = {_PORTB, _PORTB, _PORTB};
const uint8_t   main_bit[MAINPINS] = { _PIN5,  _PIN6,  _PIN7};

/* Specifies the ports and pin numbers for the columns */
/* Phantom: D1, C7, C6, D4, D0, E6, F0, F1, F4, F1, F6, F7, D7, D6, D1, D2, D3 */
/* Virulent: D0, D1, D2, D3, D4, D5, D7, E0, E1, C3, C7, F0, F1, F2, F3, F4, F5, F6, F7 */
uint8_t *const  col_ddr[NCOL] = {
          _DDRD,  _DDRD,  _DDRD,  _DDRD,  _DDRD,  _DDRD,
          _DDRD,  _DDRE,  _DDRE,  _DDRC,  _DDRC,  _DDRF,
          _DDRF,  _DDRF,  _DDRF,  _DDRF,  _DDRF,  _DDRF,
          _DDRF
};

uint8_t *const col_port[NCOL] = {
          _PORTD,  _PORTD,  _PORTD,  _PORTD,  _PORTD,  _PORTD,
          _PORTD,  _PORTE,  _PORTE,  _PORTC,  _PORTC,  _PORTF,
          _PORTF,  _PORTF,  _PORTF,  _PORTF,  _PORTF,  _PORTF,
          _PORTF
};

const uint8_t   col_bit[NCOL] = {
          _PIN0,  _PIN1,  _PIN2,  _PIN3,  _PIN4,  _PIN5,
          _PIN7,  _PIN0,  _PIN1,  _PIN3,  _PIN7,  _PIN0,
          _PIN1,  _PIN2,  _PIN3,  _PIN4,  _PIN5,  _PIN6,
          _PIN7
};

/* pressed  keeps track of which keys that are pressed
   queue    contains the keys that are sent in the HID packet
   mod_keys is the bit pattern corresponding to pressed modifier keys
   mode is the current macro mode */
bool pressed[NKEY];
uint8_t queue[7] = {255,255,255,255,255,255,255};
uint8_t mod_keys = 0;
uint8_t mode = 0;

void init(void);
void send(void);
void key_press(uint8_t key_id);
void key_release(uint8_t key_id);

int main(void) {
  uint8_t row, col, key_id;
  int countR = 0, countG = 0, countB = 0, maxR = 0, maxG = 0, maxB = 0, maxOut = 0, down = 0;
  // time_t old_time = time(NULL);
  // time_t new_time;

  init();

  for(;;) {
//    time(&new_time);
/*    if(difftime(new_time, old_time) > .05) {
      old_time = new_time;
      breathe_color(&countR, &countG, &countB, &maxR, &maxG, &maxB, &maxOut, &down, &OCR3C, &OCR3B, OCR3A);
    }
*/    _delay_ms(5);                                //  Debouncing
    for(col=0; col<NCOL; col++) {
      *col_port[col] &= ~col_bit[col];
      _delay_us(1);
      for(row=0; row<NROW; row++) {
        key_id = col*NROW+row;
        if(!(*row_port[row] & row_bit[row])) {
          if(!pressed[key_id]) {
            key_press(key_id);
            if(key_id == 0) {
              mode++;
              if(mode>=MODES) mode = 0;
              PORTC = (PORTC & 0b01111100) | ~(mode & 0b11111111);
            }
          }
        } else if(pressed[key_id])
          key_release(key_id);
      }
      *col_port[col] |= col_bit[col];
    }
  }
}

inline void send(void) {
  //return;
  uint8_t i;
  for(i=0; i<6; i++)
    keyboard_keys[i] = queue[i]<255? layout[mode][queue[i]]: 0;
  keyboard_modifier_keys = mod_keys;
  usb_keyboard_send();
}

inline void key_press(uint8_t key_id) {
  uint8_t i;
  pressed[key_id] = true;
  if(is_modifier[mode][key_id])
    mod_keys |= layout[mode][key_id];
  else {
    for(i=5; i>0; i--) queue[i] = queue[i-1];
    queue[0] = key_id;
  }
  send();
}

inline void key_release(uint8_t key_id) {
  uint8_t i;
  pressed[key_id] = false;
  if(is_modifier[mode][key_id])
    mod_keys &= ~layout[mode][key_id];
  else {
    for(i=0; i<6; i++) if(queue[i]==key_id) break;
    for(; i<6; i++) queue[i] = queue[i+1];
  }
  send();
}

void init(void) {
  uint8_t i;
  CLKPR = 0x80; CLKPR = 0;
  usb_init();
  while(!usb_configured());
  _delay_ms(1000);
  // init rows for input
  for(uint8_t row=0; row<NROW; row++) {
    *row_ddr[row] &= ~row_bit[row];
    *row_pull[row] |= row_bit[row];
  }
  // init cols for output
  for(uint8_t col=0; col<NCOL; col++) {
    *col_ddr[col] |= col_bit[col];
    *col_port[col] |= col_bit[col];
  }
  // init indicators as outputs
  for(uint8_t indicator=0; indicator<INDICATE; indicator++) {
    *ind_ddr[indicator] |= ind_bit[indicator];
    *ind_port[indicator] |= ind_bit[indicator];
  }
  // init main keyboard lights
  for(uint8_t mainpin=0; mainpin<MAINPINS; mainpin++) {
    *main_ddr[mainpin] |= main_bit[mainpin];
    *main_port[mainpin] |= main_bit[mainpin];
  }
  // init pressed array
  for(i=0; i<NKEY; i++) pressed[i] = false;

  CPU_PRESCALE(0);
  clock_portb_init(CS_clkio, WGM1_phase_correct_pwm_to_FF, COM_pwm_normal, COM_pwm_normal, COM_pwm_normal);
  clock_portc_init(CS_clkio, WGM1_phase_correct_pwm_to_FF, COM_pwm_normal, COM_pwm_normal, COM_pwm_normal);
  setColor(WHITE, &OCR3C, &OCR3B, &OCR3A);
  setColor(WHITE, &OCR1C, &OCR1B, &OCR1A);

  // TODO fixed keyboard leds.  I disabled as I cannot test them
  // LEDs are on output compare pins OC1B OC1C
  // This activates fast PWM mode on them.
  // OCR1B sets the intensity
  //TCCR1A = 0b00101001;
  //TCCR1B = 0b00001001;
  //OCR1B = OCR1C = 32;

  // LEDs: LED_A -> PORTB6, LED_B -> PORTB7
  //DDRB  &= 0b00000000;
  //PORTB &= 0b00111111;
}
