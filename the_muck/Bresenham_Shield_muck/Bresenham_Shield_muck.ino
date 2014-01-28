// include the library header
#include <glcd.h>
#include "ZyklenAutomatik_Config.h"
// include the Fonts
#include <fonts/allFonts.h>


long p = 225 ; // Steigung 2mm entspricht 200, 1.06 entspricht 106... !! Bresenham verhÃƒÂ¤ltnisse beachten!!
long v = 0;
long steps_pr = 0 ; // Festkommaarithmetik Steps fÃƒÂ¼r eine Umdrehung des

long fehler = resolution/2; 

long encoder0Pos = 0; //absolut 
int encoderWPos = 0; //Winkel
long stepper0Pos; //Position des Schrittmotors in schritten
long stepper0Pos_temp;

 //hat die Richtunt gewechselt? Wurde Umkehrspiel kompensiert?
boolean _diregtion;

long uiInterruptCountHelp=0; // hilfsZÃƒÂ¤her fÃƒÂ¼r die rpm Messung
long spindel_puls_s=0; // ZÃƒÂ¤her fÃƒÂ¼r die rpm Messung 
long ulTimeHelp, ulMillisHelp;// Zeit speicher fÃƒÂ¼r die Messung

unsigned long tmp = 0;
unsigned int Aold = 1;
unsigned int Bnew = 0;

long encoder1Pos = 0;
long tmp1 = 0;
unsigned int Aold1 = 1;
unsigned int Bnew1 = 0;

unsigned int rpm;
unsigned long rad_sec_stepper;
long distance_togo = 0;


long stepPosition = 0 ;
long stepPosition_start = 0 ;
boolean x;
int temp;
char mode;
char edit;
char menue ;

int timer1;

const char pi = 314;

static unsigned int step_count = 0;

void setup() {  
	GLCD.Init();
    pinMode(S1, INPUT);           // set pin to input
    digitalWrite(S1, HIGH);       // turn on pullup resistors
    pinMode(S2, INPUT);           // set pin to input
    digitalWrite(S2, HIGH);       // turn on pullup resistors
    pinMode(S3, INPUT);           // set pin to input
    digitalWrite(S3, HIGH);       // turn on pullup resistors
    pinMode(S4, INPUT);           // set pin to input
    digitalWrite(S4, HIGH);       // turn on pullup resistors
    pinMode(S5, INPUT);           // set pin to input
    digitalWrite(S5, HIGH);       // turn on pullup resistors
    pinMode(S6, INPUT);           // set pin to input
    digitalWrite(S6, HIGH);       // turn on pullup resistors
    pinMode(up, INPUT);           // set pin to input
    digitalWrite(up, HIGH);       // turn on pullup resistors
    pinMode(down, INPUT);           // set pin to input
    digitalWrite(down, HIGH);       // turn on pullup resistors
    pinMode(right, INPUT);           // set pin to input
    digitalWrite(right, HIGH);       // turn on pullup resistors
    pinMode(left, INPUT);           // set pin to input
    digitalWrite(left, HIGH);       // turn on pullup resistors
    pinMode(encoder, INPUT);           // set pin to input
    digitalWrite(encoder, HIGH);       // turn on pullup resistors
    
    pinMode(led1, OUTPUT);           // set pin to input
    pinMode(led2, OUTPUT);           // set pin to input
    pinMode(led3, OUTPUT);           // set pin to input
    pinMode(led4, OUTPUT);           // set pin to input
    pinMode(tweeter, OUTPUT);           // set pin to input
	/*
    pinMode(out1, OUTPUT);           // set pin to input
    pinMode(out2, OUTPUT);           // set pin to input
    pinMode(out3, OUTPUT);           // set pin to input
	*/
	
	pinMode(dirpin, OUTPUT);
	pinMode(steppin, OUTPUT);
	pinMode(encoder0PinA, INPUT);
	pinMode(encoder0PinB, INPUT);
	pinMode(encoder1PinA, INPUT);
	pinMode(encoder1PinB, INPUT);
	pinMode(encoder, INPUT);           // set pin to input
  
	
  attachInterrupt(0, doEncoderA, CHANGE); // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(1, doEncoderB, CHANGE); // encoder pin on interrupt 1 (pin 3)
  attachInterrupt(3, doPotiB, RISING ); 


  
    digitalWrite(led1, HIGH);   // sets the LED on
    digitalWrite(led2, HIGH);   // sets the LED on
    digitalWrite(led3, HIGH);   // sets the LED on
    digitalWrite(led4, HIGH);   // sets the LED on
	digitalWrite(encoder, HIGH);       // turn on pullup resistors
	/*
    digitalWrite(out1, HIGH);   // sets the LED on
    digitalWrite(out2, HIGH);   // sets the LED on
    digitalWrite(out3, HIGH);   // sets the LED on
	*/
	GLCD.SelectFont(SystemFont5x7);
	print_menue(menue);
	print_edit(edit);
	
	
	  GLCD.CursorTo(0,7);
	  GLCD.print("  <     >   <<    >> ");
	  GLCD.DrawHLine(0,50,127);
	  GLCD.DrawHLine(0,54,127);
	  GLCD.DrawVLine(3,0,49);
	  
	
	 speed_cntr_Init_Timer1();
	sei();
  
 
}
void loop(){
  //Check each changes in position
  
print_menue_numbers(menue);

if ((!digitalRead(right) || !digitalRead(left)) && status.key_pressed == FALSE && status.running == FALSE)
{
	if (!digitalRead(right))
	{
		menue++;
	}
	if (!digitalRead(left))
	{
		menue--;
	}
	if (menue >= 5)
	{menue = 0;
	}
	
	if (menue < 0)
	{menue = 4;
	}
	print_menue(menue);	
	
	status.key_pressed = TRUE;
}

if ((!digitalRead(up) || !digitalRead(down)) && status.key_pressed == FALSE && status.running == FALSE)
{
	if (!digitalRead(down))
	{
		edit++;
	}
	if (!digitalRead(up))
	{
		edit--;
	}
	if (edit >= 5 )
	{edit = 0;
	}
	
	if ( edit < 0)
	{edit = 4;
	}
	
	
	print_edit(edit);
	status.key_pressed = TRUE;
}


  

  
  
  if (!digitalRead(S5)) {mode = 0; digitalWrite(led1, HIGH);}
  if (!digitalRead(S6)) {mode = 1; digitalWrite(led1, LOW);}
	  
  if (status.running == TRUE) {digitalWrite(led4, LOW);}
	  else digitalWrite(led4, HIGH);
  
  
  if (mode == 0){
	  if (status.running == FALSE &&!digitalRead (S1)) {speed_cntr_Move(100,2000);}
	  if (status.running == FALSE &&!digitalRead (S2)) {speed_cntr_Move(-100,2000);}
	  if (status.running == FALSE &&!digitalRead (S3)) {speed_cntr_Move(1000,6000);}
	  if (status.running == FALSE &&!digitalRead (S4)) {speed_cntr_Move(-1000,6000);}
  }
    
  
	  if (mode == 1 && status.running == FALSE && !digitalRead(up) ) {status.zyklen = TRUE; stepPosition_start = stepPosition;mode = 2;status.trigger = FALSE;}
	  if (mode == 2 && status.running == FALSE && status.trigger == TRUE) {speed_cntr_Move(distance_togo,rad_sec_stepper);mode = 3;timer1 = 0;}
	  if (status.running == FALSE && mode == 3 && (stepPosition_start |= stepPosition)&& timer1 >= delay1 ) {speed_cntr_Move(-distance_togo,5000);mode = 1; status.zyklen = FALSE; }
  
  
  


   
  ulMillisHelp = millis();
    if(ulTimeHelp <= ulMillisHelp )
    {   
		if (status.zyklen == TRUE)
		{timer1++;
		}
		
		spindel_puls_s  = uiInterruptCountHelp;
		uiInterruptCountHelp=0;
		ulTimeHelp = (ulMillisHelp + 1000);
					       
    }
	
	  if (status.running == FALSE)
	  {	 steps_pr = (p * steps_mm)/100;
		 rpm =  (spindel_puls_s*60)/((resolution)) ;
		 rad_sec_stepper = (2*spindel_puls_s*pi)/(steps_pr);
	  }
	  
    
    if(!(!digitalRead(encoder)||!digitalRead(S1)||!digitalRead(S2)||!digitalRead(S3)||!digitalRead(S4)||!digitalRead(S5)||!digitalRead(S6)||!digitalRead(left)||!digitalRead(right)||!digitalRead(up)||!digitalRead(down))) {status.key_pressed = FALSE;}
;
 
}

void print_menue (int menue)
{
	
	GLCD.FillRect(0,52,127,1,WHITE);
	switch (menue){
		case 0:
		GLCD.CursorTo(1,0);
		GLCD.print("     Gewinde");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,1);
		GLCD.print("RPM:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,2);
		GLCD.print("Position:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,4);
		GLCD.print("Gew. Länge:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,5);
		GLCD.print("Steigung P:");
		
		GLCD.EraseTextLine();
		GLCD.FillRect(1,52,24,0);
		break;
		
		case 1:
		GLCD.CursorTo(1,0);
		GLCD.print("     Schleifen");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,1);
		GLCD.print("RPM:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,2);
		GLCD.print("Position:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,4);
		GLCD.print("Weg:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,5);
		GLCD.print("Geschw:");
		GLCD.EraseTextLine();
		
		GLCD.FillRect(26,52,24,0);
		break;
		
		case 2:
		GLCD.CursorTo(1,0);
		GLCD.print("     Drehen");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,1);
		GLCD.print("RPM:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,2);
		GLCD.print("Position:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,4);
		GLCD.print("Weg:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,5);
		GLCD.print("Schn. Geschw:");
		
		GLCD.EraseTextLine();
		GLCD.FillRect(51,52,24,0);
		break;
		
		case 3:
		GLCD.CursorTo(1,0);
		GLCD.print("     Bewegen");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,1);
		GLCD.print("RPM:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,2);
		GLCD.print("Position:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,4);
		GLCD.print("Eil. Geschw:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,5);
		GLCD.print("Arb. Geschw:");
		
		GLCD.EraseTextLine();
		GLCD.FillRect(76,52,24,0);
		break;
		
		case 4:
		GLCD.CursorTo(1,0);
		GLCD.print("     Obtionen");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,1);
		GLCD.print("RPM:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,2);
		GLCD.print("Position:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,4);
		GLCD.print("Eil. Geschw:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,5);
		GLCD.print("Arb. Geschw:");
		
		GLCD.EraseTextLine();
		GLCD.FillRect(101,52,24,0);
		break;


	}
	
} 
void print_edit (int edit)
{
	
	GLCD.FillRect(0,0,2,49,WHITE);
	switch (edit){
		case 0:
		
		GLCD.FillRect(1,8,0,6);
		break;
		
		case 1:
		
		GLCD.FillRect(1,16,0,6);
		break;
		
		case 2:		
		
		GLCD.FillRect(1,24,0,6);
		break;
		
		case 3:
		
		GLCD.FillRect(1,32,0,6);
		break;
		
		case 4:
		
		GLCD.FillRect(1,40,0,6);
		break;


	}
	
}

void print_menue_numbers (int menue)
{
	  switch (menue){
		  case 0:
		  if (status.running == FALSE){
			  
			  GLCD.CursorTo(14,1);
			  GLCD.PrintNumber(rpm);
			  GLCD.EraseTextLine();
			  GLCD.CursorTo(14,4);
			  GLCD.print((float)distance_togo/steps_mm,2);
			  GLCD.EraseTextLine();
			  GLCD.CursorTo(14,5);
			  GLCD.print(((float)p/100));
		  GLCD.EraseTextLine();}
		  GLCD.CursorTo(14,2);
		  GLCD.print((float)stepPosition/steps_mm);
		  GLCD.EraseTextLine();
		  break;

	  }
	  
	  switch (edit)
	  {
		  case 0:
		  distance_togo = encoder1Pos*resolution/8;
		  break;
		  case 1:
		  p = encoder1Pos;
		  break;
	  }
	
}


void doPotiB(){
	
 if (digitalRead(encoder1PinA) == digitalRead(encoder1PinB)) {
	 encoder1Pos--;
	 } else {
	 encoder1Pos++;
 }
}


// Interrupt on A changing state
void doEncoderA(){
  Bnew^Aold ? encoder0Pos++:encoder0Pos--;
  Bnew^Aold ? encoderWPos++:encoderWPos--;
  Bnew^Aold ? _diregtion = CW :_diregtion = CCW ;
  uiInterruptCountHelp++;
  
  // winkelpositionen !! 0 und resolution liegen auf einem punkt, 0 = resolution!!
  if (encoderWPos >= resolution  ){encoderWPos = 0; }
  if (encoderWPos < 0){encoderWPos = resolution-1 ;}

 if (encoderWPos == 0 && status.zyklen == TRUE && status.running == FALSE)
 {status.trigger = TRUE;
 }	
	
 if (srd.run_state == AUTO)
 {
	 fehler = fehler-steps_pr;
	 if (fehler < 0 )
	 { sm_driver_StepOutput();
		 step_count++;
		 fehler = fehler + resolution;
	 }
 }
  Aold=digitalRead(encoder0PinA); 
   
  
}


// Interrupt on B changing state
void doEncoderB(){
  Bnew=digitalRead(encoder0PinB);

  Bnew^Aold ? encoder0Pos++:encoder0Pos--;
  Bnew^Aold ? encoderWPos++:encoderWPos--;
  Bnew^Aold ? _diregtion = CW :_diregtion = CCW ;
  uiInterruptCountHelp++;
  // winkelpositionen !! 0 und resolution liegen auf einem punkt, 0 = resolution!!
  if (encoderWPos >= resolution  ){encoderWPos = 0; }
  if (encoderWPos < 0){encoderWPos = resolution-1 ;}
	 
	 if (encoderWPos  == 0 && status.zyklen == TRUE && status.running == FALSE)
	 {status.trigger = TRUE;
	 }
	
if (srd.run_state == AUTO)
{
  fehler = fehler-steps_pr;
  if (fehler < 0 )
  { sm_driver_StepOutput();
	  step_count++;
    fehler = fehler + resolution;
   }  
  }

}



void sm_driver_StepCounter(signed char inc)
{
	sm_driver_StepOutput();
}


void sm_driver_StepOutput()
{
	
	if (srd.dir == CW){
		digitalWrite(dirpin, LOW);   // sets the LED on
		digitalWrite(steppin, LOW);   // sets the LED on
		stepPosition++;
	}
	else{
		stepPosition--;
		digitalWrite(dirpin, HIGH);   // sets the LED on
		digitalWrite(steppin, LOW);   // sets the LED on
	}
	digitalWrite(steppin, HIGH);   // sets the LED on

}

void speed_cntr_Move(signed int step, unsigned int speed)
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
	
	if (srd.dir != srd.dir_old)
	{status.backlash_trigger = TRUE;		 	
	} else status.backlash_trigger = FALSE;
	
	srd.dir_old = srd.dir;
	

	// If moving only 1 step.
	if(step == 1){
		// Move one step...
		srd.accel_count = -1;
		// ...in DECEL state.
		srd.run_state = DECEL;
		// Just a short delay so main() can act on 'running'.
		srd.step_delay = 1000;
		status.running = TRUE;
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
		srd.step_delay = (T1_FREQ_148 * sqrt(A_SQ/accel_stepper))/100;

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
		accel_lim = ((long)step*accel_stepper) / ((long)accel_stepper+accel_stepper);
		// We must accelrate at least 1 step before we can start deceleration.
		if(accel_lim == 0){
			accel_lim = 1;
		}

		// Use the limit we hit first to calc decel.
		if(accel_lim <= max_s_lim){
			srd.decel_val = accel_lim - step ;
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
						
		status.running = TRUE;
		OCR1A = 10;
		// Set Timer/Counter to divide clock by 8
		TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));
		
		
	}
	
	temp = max_s_lim;
}


void speed_cntr_Init_Timer1(void)
{
		
	// Tells what part of speed ramp we are in.
	srd.run_state = STOP;
	// Timer/Counter 1 in mode 4 CTC (Not running).
	TCCR1B |= (1<<WGM13);	/***************** Original WGM12 ?!?!? *****************/
	
	// Timer/Counter 1 Output Compare A Match Interrupt enable.
	TIMSK1 |= (1<<OCIE1A);
}



ISR ( TIMER1_COMPA_vect )
{
	// Holds next delay period.
	unsigned int new_step_delay;
	// Remember the last step delay used when accelrating.
	static int last_accel_delay;
	// Counting steps when moving.
	
	// Keep track of remainder from new_step-delay calculation to incrase accurancy
	static unsigned int rest = 0;
	
	OCR1A = srd.step_delay;

	switch(srd.run_state) {
		case STOP:
		step_count = 0;
		rest = 0;
		// Stop Timer/Counter 1.
		TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));
		status.running = FALSE;
		
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
			if (status.zyklen == TRUE)
			{srd.run_state = AUTO;
			}else srd.run_state = RUN;
		}
		break;
		
		
		case RUN:
		sm_driver_StepCounter(srd.dir);
		step_count++;
		new_step_delay = srd.min_delay;
		// Chech if we should start decelration.
		if(step_count >= srd.decel_start && digitalRead (S1) && digitalRead (S2)&& digitalRead (S3) && digitalRead (S4)) 
		{
			srd.accel_count = srd.decel_val;
			// Start decelration with same delay as accel ended with.
			new_step_delay = last_accel_delay;
			srd.run_state = DECEL;
		}
		break;
		
		case AUTO: 
		if(step_count >= srd.decel_start)
		{	srd.accel_count = srd.decel_val;
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

