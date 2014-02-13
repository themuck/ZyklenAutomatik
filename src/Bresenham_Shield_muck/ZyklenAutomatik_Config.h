// include the library header
#include <glcd.h>
#include <fonts/allFonts.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define encoder_PinA 19
#define encoder_PinB 20

#define spindle_PinA 2
#define spindle_PinB 3

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
#define out3  8

#define dirpin  9 //pin fÃ¼r Richtung
#define steppin  10 //pin fÃ¼r Schritt

#define tweeter  32
 
#define resolution 800  //Resolution Encoder

#define backlash_speed (A_T_x100/1500) 
#define FSPR 400
#define steps_mm 267 // aus der Rechnungstabelle

#define pi  314

// Speed ramp Data aus der Berechnungstabelle
#define A_T_x100 1570796
#define T1_FREQ_148 13520
#define A_SQ 157079632
#define A_x20000 157

#define accel_stepper 60000 

#define default_thread_pitch 225 // Steigung 2mm entspricht 200, 1.06 entspricht 106... !!
#define default_thread_length 0
#define default_grind_way 0
#define default_grind_speed 0
#define default_cutting_way 0
#define default_cutting_speed 0
#define default_move_way 0
#define default_move_fast_speed 0
#define default_move_slow_speed 0
#define default_fast_move 6000
#define default_slow_move 1000
#define default_delay_move 2
#define default_backlash_move 200 // Schritte zur kompensation

// Speed ramp states
#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3
#define AUTO  4
#define BACKLASH 5

#define CW  0
#define CCW 1
#define TRUE 1
#define FALSE 0


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
	
	unsigned char thread:1;	
	
	unsigned char encoder_trigger:1;
	
	unsigned char backlash_trigger:1;
	
	unsigned char backlash:1;
	
	unsigned char key_pressed:1;
	
	unsigned char goback_trigger:1;
	
	unsigned char dir:1;
	
};

struct GLOBAL_FLAGS status = {FALSE, FALSE, FALSE, FALSE,FALSE,FALSE};
	
	struct config_t{
		long thread_pitch ; // Steigung 2mm entspricht 200, 1.06 entspricht 106... !!
		long thread_length ;
		long grind_way;
		long grind_speed ;
		long cutting_way ;
		long cutting_speed;
		long move_way ;
		long move_fast_speed ;
		long move_slow_speed ;
		long fast_move ;
		long slow_move ;
		long delay_move ;
		long backlash_move;
	}configuration;

