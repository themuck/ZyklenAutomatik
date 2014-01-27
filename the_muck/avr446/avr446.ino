#include <avr/io.h> 
#include <avr/interrupt.h>
#include <glcd.h>
#include <fonts/allFonts.h>

#define CW  0
#define CCW 1
#define TRUE 1
#define FALSE 0

// Speed ramp Data extern clac
#define A_T_x100 3141592.65358979
#define T1_FREQ_148 13520
#define A_SQ 314159265.358979
#define A_x20000 314.159265359
#define accel_stepper 30000

// Speed ramp states
#define STOP  0
#define ACCEL 1
#define DECEL 2
#define RUN   3

long stepPosition = 0 ;
boolean x;

typedef struct {
  //! What part of the speed ramp we are in.
  unsigned char run_state : 3;
  //! Direction stepper motor should move.
  unsigned char dir : 1;
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
	unsigned char dummy:6;
};

struct GLOBAL_FLAGS state = {FALSE, FALSE, 0};

// ARDUINO Defines for I/O


#define dirpin  9 //pin für Richtung
#define steppin  10 //pin für Schritt

#define S1  42
#define S2  43

void setup() {         
		GLCD.Init();

  pinMode(dirpin, OUTPUT);
  pinMode(steppin, OUTPUT);
  pinMode(S1, INPUT);           // set pin to input
  pinMode(S2, INPUT);           // set pin to input
  
  digitalWrite(S1, HIGH);       // turn on pullup resistors
  digitalWrite(S2, HIGH);       // turn on pullup resistors
  speed_cntr_Init_Timer1();
  sei();
	  
	  GLCD.ClearScreen();
	  GLCD.SelectFont(SystemFont5x7);
	  GLCD.print("ZyklenAutomatik");
	  
}


void loop() {
	
	
 GLCD.CursorTo(0,1);
 GLCD.print("       ");
 GLCD.CursorTo(0,1);
 GLCD.PrintNumber(stepPosition);
	  
  GLCD.CursorTo(0,2);
  switch (srd.run_state){
  case 0:  
		 GLCD.print("Stop     ");
	break;
  case 1:
		GLCD.print("Accel    ");
	break;
  case 2:
		GLCD.print("Deccl    ");
	break;
  case 3:
		GLCD.print("Run      ");
	break;}
	
	
	 //if (!digitalRead (S1)) {delay(500);speed_cntr_Move(200,2000);}
	 //if (!digitalRead (S2)) {delay(500);speed_cntr_Move(-200,2000);}
	 
	 if (srd.run_state == STOP && x == 0) {delay(500);speed_cntr_Move(400,2000); x=1;}
	 if (srd.run_state == STOP && x == 1) {delay(500);speed_cntr_Move(-400,2000); x=0;}
		 
 
  }

void sm_driver_StepCounter(signed char inc)
{
   sm_driver_StepOutput();
 }


void sm_driver_StepOutput()
{	
	digitalWrite(steppin, HIGH);   // sets the LED on	
    if (srd.dir == CW){	    
	    digitalWrite(dirpin, LOW);   // sets the LED on
		stepPosition++;
    }
    else{
	    stepPosition--;
	    digitalWrite(dirpin, HIGH);   // sets the LED on		
    }
	digitalWrite(steppin, LOW);   // sets the LED on

}

void speed_cntr_Move(signed long step, unsigned long speed)
{	
	
  //! Number of steps before we hit max speed.
  unsigned int max_s_lim;
  //! Number of steps before we must start deceleration (if accel does not hit max speed).
  unsigned int accel_lim;

  // Set direction from sign on step value.
  if(step < 0){
    srd.dir = CCW;
    step = -step;
  }
  else{
    srd.dir = CW;
  }

  // If moving only 1 step.
  if(step == 1){
    // Move one step...
    srd.accel_count = -1;
    // ...in DECEL state.
    srd.run_state = DECEL;
    // Just a short delay so main() can act on 'running'.
    srd.step_delay = 1000;
    state.running = TRUE;
    OCR1A = 10;
    // Run Timer/Counter 1 with prescaler = 8.
    TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));
  }
  // Only move if number of steps to move is not zero.
  else if(step != 0){
    // Refer to documentation for detailed information about these calculations.

    // Set max speed limit, by calc min_delay to use in timer.
    // min_delay = (alpha / tt)/ w
	srd.min_delay = A_T_x100 / speed;

    // Set accelration by calc the first (c0) step delay .
    // step_delay = 1/tt * sqrt(2*alpha/accel)
    // step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000
    srd.step_delay = (T1_FREQ_148 * sqrt(A_SQ/ accel_stepper))/100;

    // Find out after how many steps does the speed hit the max speed limit.
    // max_s_lim = speed^2 / (2*alpha*accel)
    max_s_lim = (long)speed*speed/(long)(((long)A_x20000*accel_stepper)/100);
    // If we hit max speed limit before 0,5 step it will round to 0.
	
	
    // But in practice we need to move atleast 1 step to get any speed at all.
    if(max_s_lim == 0){
      max_s_lim = 1;
    }

    // Find out after how many steps we must start deceleration.
    // n1 = (n1+n2)decel / (accel + decel)
    accel_lim = ((signed long)step*accel_stepper) / (accel_stepper+accel_stepper);
	// We must accelrate at least 1 step before we can start deceleration.
    if(accel_lim == 0){
      accel_lim = 1;
    }

    // Use the limit we hit first to calc decel.
    if(accel_lim <= max_s_lim){
      srd.decel_val = accel_lim - step;
    }
    else{
      srd.decel_val = -((long)max_s_lim*accel_stepper)/accel_stepper;
    }
    // We must decelrate at least 1 step to stop.
    if(srd.decel_val == 0){
      srd.decel_val = -1;
    }

    // Find step to start decleration.
    srd.decel_start = step + srd.decel_val;

    // If the maximum speed is so low that we dont need to go via accelration state.
    if(srd.step_delay <= srd.min_delay){
      srd.step_delay = srd.min_delay;
      srd.run_state = RUN;
    }
    else{
      srd.run_state = ACCEL;
    }
	
    // Reset counter.
    srd.accel_count = 0;
    state.running = TRUE;
    OCR1A = 10;
    // Set Timer/Counter to divide clock by 8
    TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));
  }
}


void speed_cntr_Init_Timer1(void)
{
  // Tells what part of speed ramp we are in.
  srd.run_state = STOP;
  // Timer/Counter 1 in mode 4 CTC (Not running).
  TCCR1B = (1<<WGM13);	/***************** Original WGM12 ?!?!? *****************/
  // Timer/Counter 1 Output Compare A Match Interrupt enable.
  TIMSK1 = (1<<OCIE1A);
}



ISR ( TIMER1_COMPA_vect )
{
  // Holds next delay period.
  unsigned int new_step_delay;
  // Remember the last step delay used when accelrating.
  static int last_accel_delay;
  // Counting steps when moving.
  static unsigned int step_count = 0;
  // Keep track of remainder from new_step-delay calculation to incrase accurancy
  static unsigned int rest = 0;
	
  OCR1A = srd.step_delay;

  switch(srd.run_state) {
    case STOP:
      step_count = 0;
      rest = 0;
      // Stop Timer/Counter 1.
      TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));
      state.running = FALSE;
      break;

    case ACCEL:
      sm_driver_StepCounter(srd.dir);
      step_count++;
      srd.accel_count++;
      new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) + rest)/(4 * srd.accel_count + 1));
      rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      // Chech if we should start decelration.
      if(step_count >= srd.decel_start) {
        srd.accel_count = srd.decel_val;
        srd.run_state = DECEL;
      }
      // Chech if we hitted max speed.
      else if(new_step_delay <= srd.min_delay) {
        last_accel_delay = new_step_delay;
        new_step_delay = srd.min_delay;
        rest = 0;
        srd.run_state = RUN;
      }
      break;

    case RUN:
      sm_driver_StepCounter(srd.dir);
      step_count++;
      new_step_delay = srd.min_delay;
      // Chech if we should start decelration.
      if(step_count >= srd.decel_start) {
        srd.accel_count = srd.decel_val;
        // Start decelration with same delay as accel ended with.
        new_step_delay = last_accel_delay;
        srd.run_state = DECEL;
      }
      break;

    case DECEL:
      sm_driver_StepCounter(srd.dir);
      step_count++;
      srd.accel_count++;
      new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) + rest)/(4 * srd.accel_count + 1));
      rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
      // Check if we at last step
      if(srd.accel_count >= 0){
        srd.run_state = STOP;
      }
      break;
  }
  srd.step_delay = new_step_delay;
}
