// aus der Berechnungstabelle-------------------------------------------------

#define backlash_speed (A_T_x100/1500)
#define FSPR 800 // Schritte für eine volle Umdrehung des Motors 
#define steps_mm 400 // Z Achse Schritte für 1mm 
#define stepsB_mm 400 // X Achse

#define A_T_x100 785398
#define T1_FREQ_148 13520
#define A_SQ 78539816
#define A_x20000 78

#define accel_stepper 20000 //Beschleunignug 10000 ist ein guter anfangs wert

#define resolution 1600  //(Resolution Encoder)*4, 400 PPR *4 = 1600 
// ---------------------------------------------------------------------------

// Drückt man beim Booten den Potiknopf werden diese werte in den Speicher geladen. Die manuell eingestellten werte können unter Optionen gespeichert werden.
#define default_thread_pitch 200 // Steigung 2mm entspricht 200, 1.06 entspricht 106... !!
#define default_thread_length 1000
#define default_grind_way 200
#define default_grind_speed 2000
#define default_cutting_way 200
#define default_cutting_speed 2000
#define default_move_way 200
#define default_move_fast_speed 3000
#define default_move_slow_speed 1000
#define default_fast_move 3000
#define default_slow_move 1000
#define default_delay_move 2
#define default_backlash_move 250 // Schritte zur kompensation
#define default_backlash_moveB 92 // Schritte zur kompensation
// ---------------------------------------------------------------------------

#define encoder_PinA 17
#define encoder_PinB 18
#define encoderEXT_PinA 19
#define encoderEXT_PinB 20

#define spindle_PinA 2
#define spindle_PinB 3
#define spindle_PinZ 21 


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

#define dirpin 10 //pin für Richtung Z
#define steppin  9 //pin für Schritt Z 

#define dirpinB 13 //pin für Richtung X
#define steppinB  12 //pin für Schritt X

#define tweeter  32
#define relais A7 

#define stepper_delay 10

#define pi  314

// Speed ramp states
#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3
#define AUTO  4
#define BACKLASH 5
#define ACCEL_DUMMY 6

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
} speedRampData_temp;

speedRampData_temp srd_temp;


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
		long thread_pitch ; 
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

