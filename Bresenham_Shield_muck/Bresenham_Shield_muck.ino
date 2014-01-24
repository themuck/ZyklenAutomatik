// include the library header
#include <glcd.h>
#include "ZyklenAutomatik_Config.h"

// include the Fonts
#include <fonts/allFonts.h>



long spindel_acel_steps =0;
long p = 225 ; // Steigung 2mm entspricht 200, 1.06 entspricht 106... !! Bresenham verhÃƒÂ¤ltnisse beachten!!
long steps_pr = 0 ; // Festkommaarithmetik Steps fÃƒÂ¼r eine Umdrehung des
long steps_pr_counter = 0; // Zähler für die Belscheinuging 
boolean calc_status = FALSE;


long fehler_acel = 0 ;
long fehler = fehler_def; 


volatile long encoder0Pos = 0; //absolut 
volatile long encoderWPos = 0; //Winkel
volatile long stepper0Pos; //Position des Schrittmotors in schritten

boolean _direction; // CCW oder CW 
boolean _diregtionchanged; //hat die Richtunt gewechselt? Wurde Umkehrspiel kompensiert?

unsigned long uiInterruptCountHelp=0; // hilfsZÃƒÂ¤her fÃƒÂ¼r die rpm Messung
unsigned long spindel_puls_s=0; // ZÃƒÂ¤her fÃƒÂ¼r die rpm Messung 
unsigned long ulTimeHelp, ulMillisHelp;// Zeit speicher fÃƒÂ¼r die Messung

unsigned long tmp = 0;
unsigned int Aold = 1;
unsigned int Bnew = 0;

volatile long encoder1Pos = 0;
unsigned long tmp1 = 0;
unsigned int Aold1 = 1;
unsigned int Bnew1 = 0;

boolean LEDstatustest = 0;



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
	digitalWrite(dirpin, HIGH);       // turn on pullup resistors
	digitalWrite(steppin, HIGH);       // turn on pullup resistors
	 pinMode(encoder0PinA, INPUT);
	 pinMode(encoder0PinB, INPUT);
	 pinMode(encoder1PinA, INPUT);
	 pinMode(encoder1PinB, INPUT);
	
  attachInterrupt(0, doEncoderA, CHANGE); // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(1, doEncoderB, CHANGE); // encoder pin on interrupt 1 (pin 3)
  
  attachInterrupt(4, doEncoder, RISING ); 
  
    digitalWrite(led1, HIGH);   // sets the LED on
    digitalWrite(led2, HIGH);   // sets the LED on
    digitalWrite(led3, HIGH);   // sets the LED on
    digitalWrite(led4, HIGH);   // sets the LED on
	/*
    digitalWrite(out1, HIGH);   // sets the LED on
    digitalWrite(out2, HIGH);   // sets the LED on
    digitalWrite(out3, HIGH);   // sets the LED on
	*/
	GLCD.SelectFont(SystemFont5x7);
	GLCD.print("ZyklenAutomatik");
  
 
}
void loop(){
  //Check each changes in position
static unsigned long v = 0;

 if (!digitalRead (encoder)) 
 {
	 v =p+encoder1Pos;
	  if (resolution >= (v * steps_mm) && calc_status == FALSE)
	  {
		  p=v;
		  encoder1Pos = 0;
	  }
 }
	 
 if (!digitalRead (S6)) calc_status = TRUE;
 if (!digitalRead (S5)) calc_status = FALSE;
	 
 if (calc_status == TRUE)
 {digitalWrite(led1, LOW);
 }else digitalWrite(led1, HIGH);
 
   GLCD.CursorTo(0,5);
   GLCD.print("Motor Encoder value:");
   GLCD.CursorTo(0,3);
   GLCD.print("Encoder value:");
   
   GLCD.CursorTo(0,1);
   GLCD.print("P:");
   GLCD.CursorTo(3,1);
   GLCD.PrintNumber(p+encoder1Pos);
   
   GLCD.CursorTo(0,2);
   GLCD.print("Accel:");
   GLCD.CursorTo(7,2);
   GLCD.PrintNumber(spindel_acel_steps);
   
   
   if (tmp != encoder0Pos) {
	   
	   GLCD.CursorTo(0,6);
	   GLCD.print("                    ");
	   GLCD.CursorTo(0,6);
	   GLCD.PrintNumber(encoder0Pos);
	   tmp = encoder0Pos;
   }
   if (tmp1 != encoder1Pos) {
	   
	   GLCD.CursorTo(0,4);
	   GLCD.print("                    ");
	   GLCD.CursorTo(0,4);
	   GLCD.PrintNumber(encoder1Pos);
	   tmp1 = encoder1Pos;
   }
    
    
  
    
    ulMillisHelp = millis();
    if(ulTimeHelp <= ulMillisHelp)
    {   spindel_puls_s  = uiInterruptCountHelp;
        uiInterruptCountHelp=0;
        ulTimeHelp = (ulMillisHelp + 1000); 
        if ( calc_status == FALSE && steps_pr_counter == 0){      
        steps_pr = (p * steps_mm)/100;
        spindel_acel_steps = (unsigned long) (((spindel_puls_s * spindel_puls_s)/(2*(resolution/100)))/(spindel_steps_help))*(p/100); // Berechnung der Beschleunigungsschritte
        fehler_acel = spindel_acel_steps/2; // Fehler Berechnung
               }
        
    }
  
  
  
 
}
// Interrupt on A changing state
void doEncoderA(){
  Bnew^Aold ? encoder0Pos++:encoder0Pos--;
  Bnew^Aold ? encoderWPos = encoderWPos+100:encoderWPos = encoderWPos-100;
  Bnew^Aold ? _direction = CW :_direction = CCW ;
  uiInterruptCountHelp++;
  
  // winkelpositionen !! 0 und resolution liegen auf einem punkt, 0 = resolution!!
  if (encoderWPos >= resolution  ){encoderWPos = 0; }
  if (encoderWPos < 0){encoderWPos = resolution-1 ;}
 
 if (calc_status == TRUE)
{
  if( steps_pr > steps_pr_counter)
  {   fehler_acel = fehler_acel-steps_pr;
      if (fehler_acel < 0)
      {
        steps_pr_counter++;
        fehler_acel = fehler_acel + spindel_acel_steps;        
      }     
  }  
}

if (calc_status == FALSE) 
{ 
   if( steps_pr >= steps_pr_counter && steps_pr_counter >0)
  {   fehler_acel = fehler_acel-steps_pr;
      if (fehler_acel < 0)
      {
        steps_pr_counter--;
        fehler_acel = fehler_acel + spindel_acel_steps;        
      }      
  }
 }



  fehler = fehler-(steps_pr_counter*100);
  if (fehler < 0)
  { Bnew^Aold ? digitalWrite(dirpin, HIGH):digitalWrite(dirpin, LOW);
    digitalWrite(steppin, LOW);
    delayMicroseconds(2);
    digitalWrite(steppin, HIGH);
    fehler = fehler + resolution;
    if(_direction == CW) stepper0Pos++;
    if(_direction == CCW) stepper0Pos--;
  }   
  Aold=digitalRead(encoder0PinA); 
}


// Interrupt on B changing state
void doEncoderB(){
  Bnew=digitalRead(encoder0PinB);

  Bnew^Aold ? encoder0Pos++:encoder0Pos--;
  Bnew^Aold ? encoderWPos = encoderWPos+100:encoderWPos = encoderWPos-100;
  Bnew^Aold ? _direction = CW :_direction = CCW ;
  uiInterruptCountHelp++;
  // winkelpositionen !! 0 und resolution liegen auf einem punkt, 0 = resolution!!
  if (encoderWPos >= resolution  ){encoderWPos = 0; }
  if (encoderWPos < 0){encoderWPos = resolution-1 ;}
  

 if (calc_status == TRUE)
{  if( steps_pr > steps_pr_counter)
  {   fehler_acel = fehler_acel-steps_pr;
      if (fehler_acel < 0)
      { steps_pr_counter++;
        fehler_acel = fehler_acel + spindel_acel_steps;        
      }      
  }
}

if (calc_status == FALSE) 
{ if( steps_pr >= steps_pr_counter && steps_pr_counter > 0)
  {   fehler_acel = fehler_acel-steps_pr;
      if (fehler_acel < 0)
      { steps_pr_counter--;
        fehler_acel = fehler_acel + spindel_acel_steps;        
      }      
  }
}
  fehler = fehler-(steps_pr_counter*100);
  if (fehler < 0)
  { Bnew^Aold ? digitalWrite(dirpin, HIGH):digitalWrite(dirpin, LOW);
    digitalWrite(steppin, LOW);
    delayMicroseconds(2);
    digitalWrite(steppin, HIGH);
    fehler = fehler + resolution;
    if(_direction == CW) stepper0Pos++;
    if(_direction == CCW) stepper0Pos--;
  }  
}

void doEncoder(){
	Bnew=digitalRead(encoder1PinB);
	Bnew^Aold ? encoder1Pos--:encoder1Pos++;
	Aold=digitalRead(encoder1PinA);

	
}
