#define encoder0PinA 2
#define encoder0PinB 3
#define resolution 800 // Festkommaarithmetik Reolution 
#define fehler_def 400 // Fehler = resolution/2
#define CCW 0
#define CW 1
#define TRUE 1
#define FALSE 0
#define steps_mm 200 // aus der Rechnungstabelle 
#define steps_max_acel 1600 //steps/mm^2
#define dirpin  9 //pin fÃƒÂ¼r Richtung
#define steppin  10 //pin fÃƒÂ¼r Schritt
#define spindel_steps_help (12800)// aus der Rechnungstabelle 


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

long tmp = 0; 
unsigned int Aold = 0;// Zur erkennung von Richtungswechseln 
unsigned int Bnew = 0;// Zur erkennung von Richtungswechseln 


void setup() {  
  pinMode(dirpin, OUTPUT);
  pinMode(steppin, OUTPUT);
  pinMode(encoder0PinA, INPUT); 
  pinMode(encoder0PinB, INPUT);
  attachInterrupt(0, doEncoderA, CHANGE); // encoder pin on interrupt 0 (pin 2)
  attachInterrupt(1, doEncoderB, CHANGE); // encoder pin on interrupt 1 (pin 3)
  Serial.begin (115200); // set up the Serial Connection 
  
  delay(2);
  Serial.println("You can issue commands like:");
  Serial.println("  225p - set P to 225, P= 2,25mm");
  Serial.println("  r - run");
  Serial.println("  s - stop");
  Serial.println("  x - print debug values");
  Serial.println();
}
void loop(){
  //Check each changes in position
static unsigned long v = 0;

if ( Serial.available()) 
{
    char ch = Serial.read();
    switch(ch) {
      case '0'...'9':
        v = v * 10 + ch - '0';
        break;
      case 'p':
        if (resolution >= ((v * steps_mm)/100) && calc_status == FALSE)
        {
        p=v;
        Serial.print("set P=");
        Serial.print((long)p, DEC);
        Serial.println();
        }
        else {Serial.print("stepper run or value to high P=");
              Serial.print((long)p, DEC);
              Serial.println();
              }
        v = 0;
        break;
      case 'r':
        calc_status = TRUE;
        Serial.print("run with P=");
        Serial.print((long)p, DEC);
        Serial.println();
        break;
      case 's':
        calc_status = FALSE;
         Serial.print("stop");
         Serial.println();
        break;
      case 'x':
        Serial.println(spindel_puls_s, DEC);
        Serial.println(spindel_acel_steps, DEC);
        Serial.println(steps_pr_counter, DEC);
        Serial.println(steps_pr, DEC);
        break;
    
    }
    
    
  }
    
    ulMillisHelp = millis();
    if(ulTimeHelp <= ulMillisHelp)
    {   spindel_puls_s  = uiInterruptCountHelp;
        uiInterruptCountHelp=0;
        ulTimeHelp = (ulMillisHelp + 1000); 
        if ( calc_status == FALSE && steps_pr_counter == 0){      
         steps_pr = (p * steps_mm)/100;
          spindel_acel_steps = (((spindel_puls_s * spindel_puls_s)/100)/spindel_steps_help)*p;         
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



  fehler = fehler-(steps_pr_counter);
  if (fehler < 0)
  { Bnew^Aold ? digitalWrite(dirpin, HIGH):digitalWrite(dirpin, LOW);
    digitalWrite(steppin, HIGH);
    delayMicroseconds(5);
    digitalWrite(steppin, LOW);
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
  fehler = fehler-(steps_pr_counter);
  if (fehler < 0)
  { Bnew^Aold ? digitalWrite(dirpin, HIGH):digitalWrite(dirpin, LOW);
    digitalWrite(steppin, HIGH);
    delayMicroseconds(5);
    digitalWrite(steppin, LOW);
    fehler = fehler + resolution;
    if(_direction == CW) stepper0Pos++;
    if(_direction == CCW) stepper0Pos--;
  }  
}


