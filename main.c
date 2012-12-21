#include <avr/io.h>
#include <avr/pgmspace.h>
#include "avrpwm.h"
#include "led_driver.h"
#include <util/delay.h>
#include "usb_debug_only.h"
//#include "print.h"

#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

int main(void) {
	CPU_PRESCALE(0);
	unsigned long int hexcolor = 0xFFFFFF;
//	hexcolor = ~(hexcolor & hexcolor);
    clock_portc_init(CS_clkio, WGM1_phase_correct_pwm_to_FF, COM_pwm_normal, COM_pwm_normal, COM_pwm_normal);
	DDRC = (1<<4) | (1<<5) | (1<<6) | (1<<7); //setting ports c4-7 to output
	PORTC = (1<<7); //setting port c7 to high, used for indicator LED 0, or iLED0
	// unsigned int clock1_max = 0x3FF;
	//inputs for fade_through_primaries
	int current = 0;
	int max = 0xFF;
	int state = 0;
	int down=0;
	while(1) { //main loop. no other functions should be blocking.
		if(state%2 == 0) PORTC &= ~(1<<7); //switching iLED0 between on and off
		else PORTC |= (1<<7);

		fade_through_primaries(&current, &max, &state, &down);
		// breathe_color(getRed(hexcolor), getGreen(hexcolor), getBlue(hexcolor), 0xFF);
		// cycle8();
		// cyclePWM();
	}
}
