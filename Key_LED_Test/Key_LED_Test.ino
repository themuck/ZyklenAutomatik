
/* 
...glcd\config\ks0108_Mega.h change Chip Select 
#define glcdCSEL1     34    // CS1 Bit  
#define glcdCSEL2     33    // CS2 Bit
*/


// include the library header
#include <glcd.h>

// include the Fonts
#include <fonts/allFonts.h>

#define FALSE 0
#define TRUE 1
int S1 = 42;
int S2 = 43;
int S3 = 44;
int S4 = 45;
int S5 = 46;
int S6 = 47;
int up = 51;
int down = 49;
int right = 50;
int left = 48;
int encoder = 40;
boolean LEDstatustest = 0;


int led2 = 53;
int led1 = 52;
int led3 = 30;
int led4 = 31;

int out3 = 8;
int out2 = 9;
int out1 = 10;
int tweeter = 32;

#define encoder0PinA 19
#define encoder0PinB 20


volatile int encoder0Pos = 0;
unsigned int tmp = 0;
unsigned int Aold = 1;
unsigned int Bnew = 0;

#define encoder1PinA 2
#define encoder1PinB 3

volatile int encoder1Pos = 0;
unsigned int tmp1 = 0;
unsigned int Aold1 = 1;
unsigned int Bnew1 = 0;

void setup() {
  // Initialize the GLCD 
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

  
  pinMode(out1, OUTPUT);           // set pin to input
  pinMode(out2, OUTPUT);           // set pin to input
  pinMode(out3, OUTPUT);           // set pin to input
  
  digitalWrite(led1, HIGH);   // sets the LED on
  digitalWrite(led2, HIGH);   // sets the LED on
  digitalWrite(led3, HIGH);   // sets the LED on
  digitalWrite(led4, HIGH);   // sets the LED on
  digitalWrite(out1, HIGH);   // sets the LED on
  digitalWrite(out2, HIGH);   // sets the LED on
  digitalWrite(out3, HIGH);   // sets the LED on
  
  pinMode(encoder0PinA, INPUT); 
  pinMode(encoder0PinB, INPUT);
  pinMode(encoder1PinA, INPUT); 
  pinMode(encoder1PinB, INPUT);

  attachInterrupt(4, doEncoderA, RISING );
  
 
  attachInterrupt(0, doEncoder1A, CHANGE);

  attachInterrupt(1, doEncoder1B, CHANGE);

 GLCD.ClearScreen();

  GLCD.SelectFont(SystemFont5x7);
  GLCD.print("ZyklenAutomatik");
  delay(1000);
  GLCD.CursorTo(0,2);
  GLCD.print("Outputs ON       ");
  delay(1000);
  digitalWrite(led1, LOW);   // sets the LED on
  digitalWrite(led2, LOW);   // sets the LED on
  digitalWrite(led3, LOW);   // sets the LED on
  digitalWrite(led4, LOW);   // sets the LED on
  digitalWrite(out1, LOW);   // sets the LED on
  digitalWrite(out2, LOW);   // sets the LED on
  digitalWrite(out3, LOW);   // sets the LED on
  delay(1000);
  GLCD.CursorTo(0,2);
  GLCD.print("Outputs OFF       ");
  delay(1000);
  digitalWrite(led1, HIGH);   // sets the LED on
  digitalWrite(led2, HIGH);   // sets the LED on
  digitalWrite(led3, HIGH);   // sets the LED on
  digitalWrite(led4, HIGH);   // sets the LED on
  digitalWrite(out1, HIGH);   // sets the LED on
  digitalWrite(out2, HIGH);   // sets the LED on
  digitalWrite(out3, HIGH);   // sets the LED on
  delay(1000);
  
  GLCD.CursorTo(0,2);
  GLCD.print("Tweeter test     ");
  delay(1000);
  digitalWrite(tweeter, HIGH);   // sets the LED on
  delay(1000);
  digitalWrite(tweeter, LOW);   // sets the LED on
  delay(1000);
  
    
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
   GLCD.CursorTo(0,1);
    // print the number of seconds since reset:
  GLCD.print(millis()/1000);
  
  GLCD.CursorTo(0,2);
  
   if (!digitalRead (S1)) {GLCD.print("S1 pressed"); digitalWrite(tweeter, HIGH);   }
   else if (!digitalRead (S2)) {GLCD.print("S2 pressed");}
   else if (!digitalRead (S3)) {GLCD.print("S3 pressed");}
   else if (!digitalRead (S4)) {GLCD.print("S4 pressed");}
   else if (!digitalRead (S5)) {GLCD.print("S5 pressed");LEDstatustest = FALSE;}
   else if (!digitalRead (S6)) {GLCD.print("S6 pressed");LEDstatustest = TRUE;}
   else if (!digitalRead (up)) {GLCD.print("up pressed");}
   else if (!digitalRead (down)) {GLCD.print("down pressed");}
   else if (!digitalRead (right)) {GLCD.print("right pressed");}
   else if (!digitalRead (left)) {GLCD.print("left pressed");}
   else if (!digitalRead (encoder)) {GLCD.print("Encoder pressed"); encoder0Pos = 0; encoder1Pos = 0;}
   else  
   { GLCD.CursorTo(0,2);
     GLCD.print("               ");
     digitalWrite(tweeter, LOW);   // sets the LED on
   }
  GLCD.CursorTo(0,3);
  
    GLCD.CursorTo(0,3);
    GLCD.print("Encoder value:");
    GLCD.CursorTo(0,5);
   GLCD.print("Motor Encoder value:");    
  if (tmp != encoder0Pos) {
   
    GLCD.CursorTo(0,4);
    GLCD.print("                    ");
    GLCD.CursorTo(0,4);
    GLCD.PrintNumber(encoder0Pos); 
    tmp = encoder0Pos;
       }
    if (tmp1 != encoder1Pos) {
   
    GLCD.CursorTo(0,6);
    GLCD.print("                    ");
    GLCD.CursorTo(0,6);
    GLCD.PrintNumber(encoder1Pos); 
    tmp1 = encoder1Pos;
       }
   
   if (LEDstatustest == TRUE)
   {
  digitalWrite(led1, LOW);   // sets the LED on
  digitalWrite(led2, LOW);   // sets the LED on
  digitalWrite(led3, LOW);   // sets the LED on
  digitalWrite(led4, LOW);   // sets the LED on
  digitalWrite(out1, LOW);   // sets the LED on
  digitalWrite(out2, LOW);   // sets the LED on
  digitalWrite(out3, LOW);   // sets the LED on
     
   }
  else {
  digitalWrite(led1, HIGH);   // sets the LED on
  digitalWrite(led2, HIGH);   // sets the LED on
  digitalWrite(led3, HIGH);   // sets the LED on
  digitalWrite(led4, HIGH);   // sets the LED on
  digitalWrite(out1, HIGH);   // sets the LED on
  digitalWrite(out2, HIGH);   // sets the LED on
  digitalWrite(out3, HIGH);   // sets the LED on
    
  }
 
}

// Interrupt on A changing state
void doEncoderA(){
  Bnew=digitalRead(encoder0PinB);
  Bnew^Aold ? encoder0Pos--:encoder0Pos++;
  Bnew^Aold ? digitalWrite(out2, LOW):digitalWrite(out2, HIGH);
  Aold=digitalRead(encoder0PinA);
  digitalWrite(led1, LOW);   // sets the LED on
  digitalWrite(out1, LOW);   // sets the LED on
  
}
// Interrupt on A changing state
void doEncoder1A(){
  Bnew1^Aold1 ? encoder1Pos++:encoder1Pos--;
  Aold1=digitalRead(encoder1PinA);
  digitalWrite(led2, LOW);   // sets the LED on
  
}
// Interrupt on B changing state
void doEncoder1B(){
  Bnew1=digitalRead(encoder1PinB);
  Bnew1^Aold1 ? encoder1Pos++:encoder1Pos--;
  digitalWrite(led2, LOW);   // sets the LED on
}
