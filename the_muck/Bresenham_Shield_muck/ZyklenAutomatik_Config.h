// include the library header
#include <glcd.h>
#include <fonts/allFonts.h>
#include <avr/io.h>
#include <avr/interrupt.h>


#define encoder1PinA 19
#define encoder1PinB 20

#define encoder0PinA 2
#define encoder0PinB 3

#define S1  42
#define S2  43
#define S3  44
#define S4  45
#define S5  46
#define S6  47
#define up  51
#define down  49
#define right  50
#define left  48
#define encoder  40

#define led2  53
#define led1  52
#define led3  30
#define led4  31
/*
#define out3  8
#define out2  9
#define out1  10
*/

#define tweeter  32
 
#define resolution 800  //Resolution Encoder

#define backlash 5 //in Steps
#define delay1 2

#define TRUE 1
#define FALSE 0
#define steps_mm 200 // aus der Rechnungstabelle
#define spindel_steps_help (128000)// aus der Rechnungstabelle

#define steps_max_acel 16000 //steps/mm^2

#define dirpin  9 //pin fÃ¼r Richtung
#define steppin  10 //pin fÃ¼r Schritt

#define CW  0
#define CCW 1
#define TRUE 1
#define FALSE 0

// Speed ramp Data extern clac
#define A_T_x100 3141592
#define T1_FREQ_148 13520
#define A_SQ 314159265
#define A_x20000 314
#define accel_stepper 40000

// Speed ramp states
#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3
#define AUTO  4

typedef struct {
	//! What part of the speed ramp we are in.
	unsigned char run_state : 3;
	//! Direction stepper motor should move.
	unsigned char dir : 1;
	
	unsigned char dir_old : 1;
	//! Peroid of next timer delay. At start this value set the accelration rate.
	unsigned int step_delay;
	//! What step_pos to start decelaration
	unsigned int decel_start;
	//! Sets deceleration rate.
	signed int decel_val;
	//! Minimum time delay (max speed)
	signed int min_delay;
	//! Counter used when accelerateing/decelerateing to calculate step_delay.
	signed int accel_count;
} speedRampData;

speedRampData srd;

struct GLOBAL_FLAGS {
	//! True when stepper motor is running.
	unsigned char running:1;
	//! True when uart has received a string (ended with '/r').
	unsigned char cmd:1;
	//! Dummy bits to fill up a byte.
	
	unsigned char zyklen:1;	
	
	unsigned char trigger:1;
	
	unsigned char backlash_trigger:1;
	
	unsigned char key_pressed:1;
};

struct GLOBAL_FLAGS status = {FALSE, FALSE, FALSE, FALSE,FALSE,FALSE};
