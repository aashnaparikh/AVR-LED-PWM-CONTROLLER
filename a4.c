
/* =============================================
 * ==== BEGINNING OF "DO NOT TOUCH" SECTION ====
 * =============================================
 */

#define __DELAY_BACKWARD_COMPATIBLE__ 1
#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define DELAY1 0.000001
#define DELAY3 0.01

#define PRESCALE_DIV1 8
#define PRESCALE_DIV3 64
#define TOP1 ((int)(0.5 + (F_CPU/PRESCALE_DIV1*DELAY1))) 
#define TOP3 ((int)(0.5 + (F_CPU/PRESCALE_DIV3*DELAY3)))

#define PWM_PERIOD ((long int)500)

volatile long int count = 0;
volatile long int slow_count = 0;


ISR(TIMER1_COMPA_vect) {
	count++;
}


ISR(TIMER3_COMPA_vect) {
	slow_count += 5;
}

/* =======================================
 * ==== END OF "DO NOT TOUCH" SECTION ====
 * =======================================
 */


/* *********************************************
 * **** BEGINNING OF "STUDENT CODE" SECTION ****
 * *********************************************
 */

void led_state(uint8_t LED, uint8_t state) {
    uint8_t mask = 1<<(7-(LED*2)); // Calculate the bit mask for the LED based on its position because defined biled for portl are d1,3,5,7
    if(state){
        PORTL |= mask;                  // Set the corresponding bit in PORTL to turn the LED on
    } else {                            // If state is zero, turn the LED off
        PORTL &= ~mask;                 //  by Clearing the corresponding bit in PORTL
    }
}



void SOS() {
    uint8_t light[] = {
        0x1, 0, 0x1, 0, 0x1, 0,
        0xf, 0, 0xf, 0, 0xf, 0,
        0x1, 0, 0x1, 0, 0x1, 0,
        0x0
    };

    int duration[] = {
        100, 250, 100, 250, 100, 500,
        250, 250, 250, 250, 250, 500,
        100, 250, 100, 250, 100, 250,
        250
    };

	int length = 19;
    
    for(int i = 0; i < length; i++) {
        uint8_t pattern = light[i];

        switch(pattern){
            case 0x0: //all leds off
                _delay_ms(duration[i]); //just delay
                break;


            default: //turn on leds
            for(uint8_t j = 0; j < 4; j++) {
                led_state(j, (pattern >> j) & 1); // Extract and apply the state of each LED
            }
            _delay_ms(duration[i]); // Wait for the specified duration

            for(uint8_t j = 0; j < 4; j++) {
                led_state(j, 0); // Turn off all LEDs
            }
            break;
        }
    }    
}


void glow(uint8_t LED, float brightness) {
    int threshold = PWM_PERIOD * brightness; // Calculate the threshold for the PWM signal
    uint8_t is_on = 0 ; // Variable to track the LED state
    while(1){
        
        if(count>=PWM_PERIOD){
            count = 0; //rest pwm counter
        }
        if(count<threshold) {
           if(!is_on){
            led_state(LED, 1); // Turn the LED on
            is_on = 1; // Update the state
            
           }
        }
        else{
            if(is_on) {
                led_state(LED, 0); // Turn the LED off; decrease brightness gradually
                is_on = 0; // Update the state
            }
        }
        
    }
    
    _delay_us(50); //small delay to give visible pwm timing; the delay was added to mainatin smoothness for the brightness changes
}



void pulse_glow(uint8_t LED) {
        
    int threshold = 0;
    int fade_in = 1; // 1 for increasing brightness, -1 for decreasing

    while (1) {
        // Only update threshold when slow_count changes
        static int last_slow_count = -1;
        if (slow_count != last_slow_count) {
            last_slow_count = slow_count;
            threshold += fade_in;

            if (threshold >= PWM_PERIOD) {
                threshold = PWM_PERIOD;
                fade_in = -1; // start fading out;because it reached it max brightness so fade out
            } else if (threshold <= 0) {
                threshold = 0;
                fade_in= 1; // start fading in;because it reached it minium brightness tso fade in
            }
        }

        // PWM loop;it increments count every time so to need to rest count seperately
        for (int count = 0; count< PWM_PERIOD; count++) {
            if (count < threshold) {
                led_state(LED, 1); // LED on
            } else {
                led_state(LED, 0); // LED off
            }
            _delay_us(1); // 1 microsecond delay for smooth PWM
        }
    }

    
}


void light_show() {
    uint8_t light[] = {
        0b00001111, 0b00000000, 0b00001111, 0b00000000, 0b00001111, 0b00000000,
        0b00000110, 0b00000000, 0b00001001, 0b00000000, 0b00001111, 0b00000000,
        0b00001111, 0b00000000, 0b00001111, 0b00000000, 0b00001001, 0b00000000,
        0b00000110, 0b00000000, 0b00001000, 0b00001100, 0b00000110, 0b00000011,
        0b00000001, 0b00000011, 0b00000110, 0b00001100, 0b00001000, 0b00001100,
        0b00000110, 0b00000011, 0b00000001, 0b00000011, 0b00000110, 0b00001111,
        0b00000000, 0b00001111, 0b00000000, 0b00000110, 0b00000000, 0b00000110,
        0b00000000
    };

    // Array defining the duration of each pattern in milliseconds
    int duration[] = {
        250, 250, 250, 250, 250, 250,
        100, 100, 100, 100, 250, 250,
        250, 250, 250, 250, 100, 100,
        100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 250,
        250, 250, 250, 250, 250, 250,
        250
    };

    int length = 43; // Total number of patterns in the light show

   
    //for loop implemented as done in part b ;SOS with changes made as required for light show
    for(int i = 0; i < length; i++) {
        uint8_t pattern = light[i];

            for(uint8_t j = 0; j < 4; j++) {
                led_state(j, (pattern >> j) & 1); // Extract and apply the state of each LED
            }
            _delay_ms(duration[i]); // Wait for the specified duration

            if (pattern != 0b00000000) {
                // Turn all LEDs off if the pattern is not 0
                for (uint8_t j = 0; j < 4; j++) {
                    led_state(j, 0); // Turn off all LEDs
                }
            }
    }
}


/* ***************************************************
 * **** END OF FIRST "STUDENT CODE" SECTION **********
 * ***************************************************
 */


/* =============================================
 * ==== BEGINNING OF "DO NOT TOUCH" SECTION ====
 * =============================================
 */

int main() {
    /* Turn off global interrupts while setting up timers. */

	cli();

	/* Set up timer 1, i.e., an interrupt every 1 microsecond. */
	OCR1A = TOP1;
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR1B |= (1 << WGM12);
    /* Next two lines provide a prescaler value of 8. */
	TCCR1B |= (1 << CS11);
	TCCR1B |= (1 << CS10);
	TIMSK1 |= (1 << OCIE1A);

	/* Set up timer 3, i.e., an interrupt every 10 milliseconds. */
	OCR3A = TOP3;
	TCCR3A = 0;
	TCCR3B = 0;
	TCCR3B |= (1 << WGM32);
    /* Next line provides a prescaler value of 64. */
	TCCR3B |= (1 << CS31);
	TIMSK3 |= (1 << OCIE3A);


	/* Turn on global interrupts */
	sei();

/* =======================================
 * ==== END OF "DO NOT TOUCH" SECTION ====
 * =======================================
 */


/* *********************************************
 * **** BEGINNING OF "STUDENT CODE" SECTION ****
 * *********************************************
 */
    
    DDRL = 0b10101010; //set led pins to output
    DDRB |= (1<<PB1)|(1<<PB3); //set leds to portb
 //This code could be used to test your work for part A.
    /*led_state(0, 1);
	_delay_ms(1000);
	led_state(2, 1);
	_delay_ms(1000);
	led_state(1, 1);
	_delay_ms(1000);
	led_state(2, 0);
	_delay_ms(1000);
	led_state(0, 0);
	_delay_ms(1000);
	led_state(1, 0);
	_delay_ms(1000);
 */

/* This code could be used to test your work for part B.*/

	//SOS();
 

// This code could be used to test your work for part C.

	//glow(2, .01);
    //glow(2,0.1);
    //glow(2,0.5);
    //glow(2,0.9);



/* This code could be used to test your work for part D.*/

	//pulse_glow(3);
 


/* This code could be used to test your work for the bonus part*/

	//light_show();
 

/* ****************************************************
 * **** END OF SECOND "STUDENT CODE" SECTION **********
 * ****************************************************
 */
}
