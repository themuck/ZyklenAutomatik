// use AVR446 - Linear speed control of stepper motor Documentation, Atmel AVR Application Note
// ZykloenAutomatik is Open Source under CC BY-NC-SA 3.0 
// https://github.com/themuck/ZyklenAutomatik

/* TO DO 
- Aufräumen
- Kommentare
- delay(); Funktion entfernen
- Shönere Struktur, If anweisungen - > switch / case 
- Programm Verriegelungen 
- Entprellroutine ?
- Winkel Positions Anzege
- Visualisierung LEDs, Töne 
- Variablen größe
- Bresenham rirchtung
- Zahlenwerte Negativ verriegeln 
-....
*/

#include <glcd.h>
#include "ZyklenAutomatik_Config.h"
#include <fonts/allFonts.h>
#include "EEPROMAnything.h"
#include <EEPROM.h>
#include <avr/io.h>
#include <avr/interrupt.h>

int zeta[][4] = {                 {  0, -1, +1,  0 },
                                  { +1,  0,  0, -1 },
                                  { -1,  0,  0, +1 },
                                  {  0, +1, -1,  0 }
                                };
int encRaster = 4;    // Rasterstopps: 1,2 oder 4
int encState  = 0;    // Aktueller Zustand
int encDelta  = 0;    // Positionsveränderung
int encAB  =0  ;         // Eingabe aus A und B

long fehler = resolution/2; 

long spindle_posi = 0; //absolut 
int spindle_angle = 0; //Winkel
boolean spindle_dir;
int spindle_start;
int spindle_flag;
unsigned int spindle_rpm = 0;
char rotation ;

long uiInterruptCountHelp=0; // hilfsZÃƒÂ¤her fÃƒÂ¼r die rpm Messung
long spindel_puls_s=0; // ZÃƒÂ¤her fÃƒÂ¼r die rpm Messung 
long ulTimeHelp, ulMillisHelp;// Zeit speicher fÃƒÂ¼r die Messung

boolean spindle_Aold = 1;
boolean spindle_Bnew = 0;

long encoder_posi = 0;
long encoderEXT_posi = 0;

boolean encoderEXT_dir ;

unsigned int backlashB_count = 0;	

boolean encoder_Aold = 1;
boolean encoder_Bnew = 0;
boolean encoderEXT_Aold = 1;
boolean encoderEXT_Bnew = 0;

long stepperB_posi = 0;

long stepper_rad_sec;
long stepper_posi = 0;
long stepper_steps_pr = 0 ; 
long stepper_posi_tmp = 0 ;
boolean x;
int temp;
char mode ;
char mode2 ;
char edit;
char menue ;
long steps_toaccel;

int timer1;

static unsigned int step_count = 0;
	// Holds next delay period.
unsigned int new_step_delay;
	// Remember the last step delay used when accelrating.
static int last_accel_delay;

static unsigned int step_count_temp = 0;
// Holds next delay period.
unsigned int new_step_delay_temp;
// Remember the last step delay used when accelrating.
static int last_accel_delay_temp;

void setup() {  
	io_init(); 
		
	attachInterrupt(5, doEncoderB, FALLING);	 
	attachInterrupt(0, doSpindleA, CHANGE);
	attachInterrupt(1, doSpindleB, CHANGE);
        attachInterrupt(2, doSpindleZ, FALLING);
        attachInterrupt(3, doEncoderEXTA, CHANGE);
        attachInterrupt(4, doEncoderEXTB, CHANGE);
  
	GLCD.Init();  	
	GLCD.SelectFont(SystemFont5x7);
	GLCD.CursorTo(3,2);
	GLCD.print("ZyklenAutomatik");
	GLCD.EraseTextLine();
		GLCD.CursorTo(3,5);
		GLCD.print("CC BY-NC-SA 3.0");
		GLCD.EraseTextLine();
	delay(2000);
	speed_cntr_Init_Timer1();
	
	
	if (!digitalRead(encoder)) // Lade Defalut werte und nicht den Speicher
	{	digitalWrite(led2,LOW);
		GLCD.CursorTo(0,3);
		GLCD.print("load default values");
		GLCD.EraseTextLine();
		while(!digitalRead(encoder));
		delay(1000);
		digitalWrite(led2,HIGH);
		write_default_config();
		
	}else EEPROM_readAnything(0, configuration);
  
	 print_menue();
	 print_edit_cursor();
	 //-------------------------- Draw Lines --------------------------------
	 GLCD.DrawHLine(0,50,127);
	 GLCD.DrawHLine(0,54,127);
	 GLCD.DrawVLine(3,0,49);

}
void loop(){
 
  
print_menue_numbers();

if ((!digitalRead(right) || !digitalRead(left)) && status.key_pressed == FALSE && status.running == FALSE&& mode == 0)
{	mode2 = 0; // Automatiken zurück setzen 
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
	print_menue();		
	status.key_pressed = TRUE;
}

if ((!digitalRead(up) || !digitalRead(down)) && status.key_pressed == FALSE && status.running == FALSE )
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
	print_edit_cursor();
	status.key_pressed = TRUE;
}


  // daten speichern ---------------------------------
  if (!digitalRead(encoder) && menue == 4 && edit == 4 && status.key_pressed == FALSE) {
	  digitalWrite(led2, LOW);
	  EEPROM_writeAnything(0, configuration);
	  status.key_pressed = TRUE;delay(1000);
	  digitalWrite(led2, HIGH);}	
	  
  // nummern auf 0 setzen------------------------------  
  if (!digitalRead(encoder) && status.key_pressed == FALSE) {write_edit_number(0);status.key_pressed = TRUE;}
  // Mode Wechsel-------------------------------------- 
  if (!digitalRead(S5)&& status.key_pressed == FALSE&& status.running == FALSE ) {mode = 0; print_menue();digitalWrite(led1, HIGH);status.key_pressed = TRUE;}
  if (!digitalRead(S6)&& menue != 4&& status.key_pressed == FALSE&& status.running == FALSE ) {mode = 1; print_menue();digitalWrite(led1, LOW);status.key_pressed = TRUE;}
	  
	  
  if (status.running == TRUE) {digitalWrite(led4, LOW);}
	  else digitalWrite(led4, HIGH);
  
    // *****************************************************Programm Funktionen--------------------------------------------------------------------------  
	
  if (mode == 0 && menue != 4){
	  if (status.running == FALSE &&!digitalRead (S1)) {speed_cntr_Move(-100,configuration.slow_move);}
	  if (status.running == FALSE &&!digitalRead (S2)) {speed_cntr_Move(100,configuration.slow_move);}
	  if (status.running == FALSE &&!digitalRead (S3)) {speed_cntr_Move(-1000,configuration.fast_move);}
	  if (status.running == FALSE &&!digitalRead (S4)) {speed_cntr_Move(1000,configuration.fast_move);}
		}
    
   // Gewinde --------------------------------------------------------
	if (mode == 1 && menue == 0){
			if (status.running == TRUE &&(!digitalRead (S2) || !digitalRead (S3))&& status.key_pressed == FALSE) {decl_trigger();status.key_pressed = TRUE; status.goback_trigger = FALSE;}

			if (status.running == FALSE && !digitalRead (S4)&& status.key_pressed == FALSE && (stepper_posi_tmp-stepper_posi)!= stepper_posi) {speed_cntr_Move((stepper_posi_tmp-stepper_posi),configuration.fast_move);status.goback_trigger = FALSE;status.key_pressed = TRUE;}	
				
			if (status.running == FALSE && !digitalRead (S1)&& status.key_pressed == FALSE && status.thread == FALSE) {
				
				stepper_posi_tmp = stepper_posi;
				if (configuration.thread_length > 0)
				{speed_cntr_Move(-steps_toaccel,configuration.fast_move);
				}
				if (configuration.thread_length < 0)
				{speed_cntr_Move(steps_toaccel,configuration.fast_move);
				}
				
				delay(1000);
				spindle_flag = 1;
				
				status.thread = TRUE; status.goback_trigger = TRUE;status.key_pressed = TRUE; 
				
				if (configuration.thread_length > 0) speed_cntr_Move(configuration.thread_length+steps_toaccel,stepper_rad_sec);
				if (configuration.thread_length < 0) speed_cntr_Move(configuration.thread_length-steps_toaccel,stepper_rad_sec);
				
				
				}	
		    //  Auto---------------		
		    if (status.running == FALSE && stepper_posi == (stepper_posi_tmp+configuration.thread_length) && status.goback_trigger == TRUE && mode2 == 1){delay(configuration.delay_move);speed_cntr_Move((stepper_posi_tmp-stepper_posi),configuration.fast_move); status.thread = FALSE; status.goback_trigger = FALSE;}
			
			// Toogle Auto---------------
			if (!digitalRead (left)&& status.key_pressed == FALSE&& status.running == FALSE ) {mode2 = 0; print_menue();status.key_pressed = TRUE;status.thread = FALSE; status.goback_trigger = FALSE;}
			if (!digitalRead (right)&& status.key_pressed == FALSE&& status.running == FALSE ) {mode2 = 1; print_menue();status.key_pressed = TRUE;status.thread = FALSE; status.goback_trigger = FALSE;}
										
		}
		
		// Schleifen --------------------------------------
		if (mode == 1 && menue == 1){
			if (status.running == TRUE &&(!digitalRead (S2) || !digitalRead (S3))&& status.key_pressed == FALSE) {decl_trigger();status.key_pressed = TRUE; status.goback_trigger = FALSE;}

			if (status.running == FALSE &&!digitalRead (S4)&& status.key_pressed == FALSE && (stepper_posi_tmp-stepper_posi)!= stepper_posi) {speed_cntr_Move((stepper_posi_tmp-stepper_posi),configuration.grind_speed);status.goback_trigger = FALSE;status.key_pressed = TRUE;}
			
			if (status.running == FALSE && !digitalRead (S1)&& status.key_pressed == FALSE && status.goback_trigger == FALSE) {status.goback_trigger = TRUE;status.key_pressed = TRUE;stepper_posi_tmp = stepper_posi; speed_cntr_Move(configuration.grind_way,configuration.grind_speed);status.dir = FALSE;}
			//  Auto---------------
			if(mode2 == 1 && status.goback_trigger == TRUE && status.running == FALSE ){ 
			if (status.dir == FALSE && status.running == FALSE ) {speed_cntr_Move(-configuration.grind_way,configuration.grind_speed);status.dir = TRUE;}
			if (status.dir == TRUE && status.running == FALSE ) {speed_cntr_Move(configuration.grind_way,configuration.grind_speed);status.dir = FALSE;}
			}
			
			// Toogle Auto---------------
			if (!digitalRead (left)&& status.key_pressed == FALSE&& status.running == FALSE ) {mode2 = 0; print_menue();status.key_pressed = TRUE; status.goback_trigger = FALSE;}
			if (!digitalRead (right)&& status.key_pressed == FALSE&& status.running == FALSE ) {mode2 = 1; print_menue();status.key_pressed = TRUE; status.goback_trigger = FALSE;}
						
			}
			
			
//		 Drehen -----------------------------------------	
		if (mode == 1 && menue == 2){ 
			
			if ((!digitalRead (S2) || !digitalRead (S3))&& status.key_pressed == FALSE) {decl_trigger();;status.key_pressed = TRUE;}
				
			if (status.running == FALSE &&!digitalRead (S1)&& status.key_pressed == FALSE) {stepper_posi_tmp = stepper_posi;speed_cntr_Move (configuration.cutting_way,stepper_rad_sec);status.key_pressed = TRUE;}
			if (status.running == FALSE &&!digitalRead (S4)&& status.key_pressed == FALSE) {speed_cntr_Move ((stepper_posi_tmp-stepper_posi),configuration.fast_move);status.key_pressed = TRUE;}
										
		} 

		// Bewegen ----------------------------------------
		if (mode == 1 && menue == 3){
			
				  if (status.running == FALSE &&!digitalRead (S1)&& status.key_pressed == FALSE) {speed_cntr_Move(-configuration.move_way,configuration.move_slow_speed);status.key_pressed = TRUE;}
				  if (status.running == FALSE &&!digitalRead (S2)&& status.key_pressed == FALSE) {speed_cntr_Move(+configuration.move_way,configuration.move_slow_speed);status.key_pressed = TRUE;}
				  if (status.running == FALSE &&!digitalRead (S3)&& status.key_pressed == FALSE) {speed_cntr_Move(-configuration.move_way,configuration.move_fast_speed);status.key_pressed = TRUE;}
				  if (status.running == FALSE &&!digitalRead (S4)&& status.key_pressed == FALSE) {speed_cntr_Move(+configuration.move_way,configuration.move_fast_speed);status.key_pressed = TRUE;}
		}
		
		
		

   
    ulMillisHelp = millis();
    if(ulTimeHelp <= ulMillisHelp )
    {   		
		spindel_puls_s  = uiInterruptCountHelp;
		uiInterruptCountHelp=0;
		ulTimeHelp = (ulMillisHelp + 1000);
		   
		   spindle_rpm =  (spindel_puls_s*60)/(resolution) ;
          if(status.running == FALSE){
		   if (menue == 0) stepper_steps_pr = ((long)configuration.thread_pitch * steps_mm)/100;
		   if (menue == 2) stepper_steps_pr = ((long)configuration.cutting_speed * steps_mm)/100;
		   stepper_rad_sec = ((long)2*spindle_rpm*pi*stepper_steps_pr)/((long)FSPR*60);
		   steps_toaccel = (long)stepper_rad_sec*stepper_rad_sec/(long)(((long)A_x20000*accel_stepper)/100);
		   }
					       
    }	
	  
encAB     = (!digitalRead(encoder_PinA) << 1) + !digitalRead(encoder_PinB);
encDelta += zeta[encState][encAB];
encState  = encAB;
if( encDelta == encRaster || encDelta == -encRaster )
{   encDelta > 0 ? encoder_posi++ : encoder_posi--;
    encDelta > 0 ? trigger_edit_number(+1) : trigger_edit_number(-1);
    encDelta = 0;
}  

    if(!(!digitalRead(encoder)||!digitalRead(S1)||!digitalRead(S2)||!digitalRead(S3)||!digitalRead(S4)||!digitalRead(S5)||!digitalRead(S6)||!digitalRead(left)||!digitalRead(right)||!digitalRead(up)||!digitalRead(down)||!digitalRead(encoder))) {status.key_pressed = FALSE;}
;
 
}

/*
Menue = 

0 Gewinde
1 Schleifen
2 Drehen 
3 Bewegen
4 Optionen 

*/

void print_menue ()
{
	
	GLCD.FillRect(0,52,127,1,WHITE);
	switch (menue){
		case 0:
		GLCD.CursorTo(1,0);
		GLCD.print("     Gewinde        ");
              	GLCD.EraseTextLine();
		GLCD.CursorTo(1,1);
		GLCD.print("RPM:");
		GLCD.EraseTextLine();
		
		GLCD.CursorTo(1,3);
		//GLCD.print("Position X:");
		GLCD.EraseTextLine();	
		
		GLCD.CursorTo(1,2);
		GLCD.print("Position Z:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,4);
		GLCD.print("Gew. Laenge:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,5);
		GLCD.print("Steigung P:");
		
		GLCD.EraseTextLine();
		GLCD.FillRect(1,52,24,0);
		
		if(mode==0){
		GLCD.CursorTo(0,7);
		GLCD.print("  <     >   <<    >> ");
		GLCD.EraseTextLine();
		}else
		{
			GLCD.CursorTo(0,7);
			GLCD.print("  GO  |  Stop  | Ret.");
			GLCD.EraseTextLine();
			
			if(mode2==0){
				GLCD.CursorTo(1,0);
				GLCD.print("<    Ret. manual   >");
				GLCD.EraseTextLine();
			}else
			{
				GLCD.CursorTo(1,0);
				GLCD.print("<    Ret. auto     >");
				GLCD.EraseTextLine();
			}
			
		}
		
		
		
		break;
		
		case 1:
		GLCD.CursorTo(6,0);
		GLCD.CursorTo(1,0);
		GLCD.print("     Schleifen      ");
                GLCD.EraseTextLine();
		GLCD.CursorTo(1,1);
		GLCD.print("RPM:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,2);
		GLCD.print("Position:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,3);
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,4);
		GLCD.print("Weg:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,5);
		GLCD.print("Geschw:");
		GLCD.EraseTextLine();
		
		GLCD.FillRect(26,52,24,0);
		if(mode==0){
			GLCD.CursorTo(0,7);
			GLCD.print("  <     >   <<    >> ");
			GLCD.EraseTextLine();
			GLCD.CursorTo(1,3);
			GLCD.EraseTextLine();
		}else
		{
			GLCD.CursorTo(0,7);
			GLCD.print("  GO  |  Stop  | Ret.");
			GLCD.EraseTextLine();
			
			if(mode2==0){
				GLCD.CursorTo(1,0);
				GLCD.print("<    Ret. manual   >");
				GLCD.EraseTextLine();
			}else
			{
				GLCD.CursorTo(1,0);
				GLCD.print("<       Auto       >");
				GLCD.EraseTextLine();
			}
			
		}
		break;
		
		case 2:
		GLCD.CursorTo(1,0);
		GLCD.print("     Drehen         ");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,1);
		GLCD.print("RPM:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,2);
		GLCD.print("Position:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,3);
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,4);
		GLCD.print("Weg:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,5);
		GLCD.print("Vorschup f:");
		
		GLCD.EraseTextLine();
		GLCD.FillRect(51,52,24,0);
		if(mode==0){
			GLCD.CursorTo(0,7);
			GLCD.print("  <     >   <<    >> ");
			GLCD.EraseTextLine();
		}else
		{
			GLCD.CursorTo(0,7);
			GLCD.print("< GO  |  Stop  | Ret.");
			GLCD.EraseTextLine();
		}
		break;
		
		case 3:
		GLCD.CursorTo(1,0);
		GLCD.print("     Bewegen        ");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,1);
		GLCD.print("RPM:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,3);
		GLCD.print("Weg:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,2);
		GLCD.print("Position:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,4);
		GLCD.print("Eil G.:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,5);
		GLCD.print("Arb G.:");
		
		GLCD.EraseTextLine();
		GLCD.FillRect(76,52,24,0);
			if(mode==0){
				GLCD.CursorTo(0,7);
				GLCD.print("  <     >   <<    >> ");
				GLCD.EraseTextLine();
			}else
			{
				GLCD.CursorTo(0,7);
				GLCD.print("  <     >   <<    >> ");
				GLCD.EraseTextLine();
			}
		break;
		
		case 4:
		GLCD.CursorTo(1,0);
		GLCD.print("     Optionen       ");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,1);
		GLCD.print("Verz.:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,2);
		GLCD.print("Eil G.:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,3);
		GLCD.print("Arb G.:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,4);
		GLCD.print("Spiel:");
		GLCD.EraseTextLine();
		GLCD.CursorTo(1,5);
		GLCD.print("Speichern?");
		
		GLCD.EraseTextLine();
		GLCD.FillRect(101,52,24,0);
		GLCD.CursorTo(0,7);
		GLCD.EraseTextLine();
		break;


	}
	
} 


void print_edit_cursor ()
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

void print_menue_numbers ()
{
	  switch (menue){
		  case 0: // Gewinde ---------------------------
		  if (status.running == FALSE){
			 GLCD.CursorTo(14,1);
			GLCD.PrintNumber(spindle_rpm);
			GLCD.EraseTextLine();
				  
			  GLCD.CursorTo(14,4);
			  GLCD.print((float)configuration.thread_length/steps_mm);
			  GLCD.EraseTextLine();
			  GLCD.CursorTo(14,5);
			  GLCD.print(((float)configuration.thread_pitch/100));
			  GLCD.EraseTextLine();
			  }
			  GLCD.CursorTo(14,2);
			  GLCD.print((float)stepper_posi/steps_mm);
			  
			  GLCD.EraseTextLine();
	         GLCD.CursorTo(14,3);
			//GLCD.print((float)stepperB_posi/stepsB_mm);
			GLCD.EraseTextLine();
				  
		  
		  break;
		  
		    case 1:// Schleifen ---------------------------
		    if (status.running == FALSE){
			    
			 GLCD.CursorTo(14,1);
			 GLCD.PrintNumber(spindle_rpm);
			 GLCD.EraseTextLine();
			
			 GLCD.CursorTo(14,4);
			 GLCD.print((float)configuration.grind_way/steps_mm);
			 GLCD.EraseTextLine();
			 GLCD.CursorTo(14,5);
			 GLCD.print((float)((long)configuration.grind_speed*FSPR)/((long)2*pi*steps_mm) );
		     GLCD.EraseTextLine();
			 }
			  GLCD.CursorTo(14,2);
			  GLCD.print((float)stepper_posi/steps_mm);
			  GLCD.EraseTextLine();
		     
		    break;
			
			    case 2:
			    if (status.running == FALSE){
				    
				    GLCD.CursorTo(14,1);
				    GLCD.PrintNumber(spindle_rpm);
				    GLCD.EraseTextLine();
				    
				    GLCD.CursorTo(14,4);
				    GLCD.print((float)configuration.cutting_way/steps_mm);
				    GLCD.EraseTextLine();
				    GLCD.CursorTo(14,5);
				    GLCD.print((float)((long)configuration.cutting_speed*FSPR)/((long)2*pi*steps_mm) );
				    GLCD.EraseTextLine();
			    }
				GLCD.CursorTo(14,2);
				GLCD.print((float)stepper_posi/steps_mm);
				GLCD.EraseTextLine();
			    
			    break;
				
				    case 3:
				    if (status.running == FALSE){
					    
					    GLCD.CursorTo(14,1);
					    GLCD.PrintNumber(spindle_rpm);
					    GLCD.EraseTextLine();
					   
						GLCD.CursorTo(14,3);
						GLCD.print((float)configuration.move_way/steps_mm);
						GLCD.EraseTextLine();
					    GLCD.CursorTo(14,4);
					    GLCD.print((float)((long)configuration.move_fast_speed*FSPR)/((long)2*pi*steps_mm) );
					    GLCD.EraseTextLine();
					    GLCD.CursorTo(14,5);
					    GLCD.print((float)((long)configuration.move_slow_speed*FSPR)/((long)2*pi*steps_mm) );
					    GLCD.EraseTextLine();
				    }
					 GLCD.CursorTo(14,2);
					 GLCD.print((float)stepper_posi/steps_mm);
					 GLCD.EraseTextLine();
				    
				    break;
					
					  case 4:
					  if (status.running == FALSE){
						  
						  GLCD.CursorTo(14,1);
						  GLCD.print((float)configuration.delay_move/1000);
						  GLCD.EraseTextLine();  
						  GLCD.CursorTo(14,4);
						  GLCD.print(configuration.backlash_move);
						  GLCD.EraseTextLine();
						  GLCD.CursorTo(14,2);
						  GLCD.print((float)((long)configuration.fast_move*FSPR)/((long)2*pi*steps_mm) );
						  GLCD.EraseTextLine();
						  GLCD.CursorTo(14,3);
						  GLCD.print((float)((long)configuration.slow_move*FSPR)/((long)2*pi*steps_mm) );
						  GLCD.EraseTextLine();
					  }
					  
					  
					  break;

	  }
	 	
}

void doEncoderB(){
  
rotation = TRUE;
 
}
// Interrupt on A changing state
void doSpindleA(){
  spindle_Bnew^spindle_Aold ? spindle_posi++:spindle_posi--;
  spindle_Bnew^spindle_Aold ? spindle_angle++:spindle_angle--;
  spindle_Bnew^spindle_Aold ? spindle_dir = CW :spindle_dir = CCW ;
  uiInterruptCountHelp++;

 // Einkoppeln der Spindel
 if (spindle_angle == spindle_start && status.thread == TRUE && status.backlash == FALSE && spindle_flag == 3)
 {	 srd.run_state = ACCEL;
	 srd.accel_count = 0;
	 
	 status.running = TRUE;
	 OCR1A = 10;
	 // Set Timer/Counter to divide clock by 8
	 TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));
 }	
	
 if (srd.run_state == AUTO)
 {	
 
	 fehler = fehler-stepper_steps_pr;
	 if (fehler < 0 )
	 { sm_driver_StepOutput();
		 step_count++;
		 fehler = fehler + resolution;
	 }
	 new_step_delay = srd.min_delay;
		if(step_count >= srd.decel_start) // Abbruch bedingung
		{	decl_trigger();
		}
 }
  spindle_Aold=digitalRead(spindle_PinA); 
   
  
}
// Interrupt on B changing state
void doSpindleB(){
  spindle_Bnew=digitalRead(spindle_PinB);

  spindle_Bnew^spindle_Aold ? spindle_posi++:spindle_posi--;
  spindle_Bnew^spindle_Aold ? spindle_angle++:spindle_angle--;
  spindle_Bnew^spindle_Aold ? spindle_dir = CW :spindle_dir = CCW ;
  uiInterruptCountHelp++;
  // winkelpositionen !! 0 und resolution liegen auf einem punkt, 0 = resolution!!

// Einkoppeln der Spindel 
	 if (spindle_angle == spindle_start && status.thread == TRUE && status.backlash == FALSE && spindle_flag == 3)
	 {	 srd.run_state = ACCEL;
		 srd.accel_count = 0;
		 
		 status.running = TRUE;
		 OCR1A = 10;
		 // Set Timer/Counter to divide clock by 8
		 TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));
	 }
	

 if (srd.run_state == AUTO)
 { 
	 
	 fehler = fehler-stepper_steps_pr;
	 if (fehler < 0 )
	 { sm_driver_StepOutput();
		 step_count++;
		 fehler = fehler + resolution;
	 }
	 
	 new_step_delay = srd.min_delay;
	 if(step_count >= srd.decel_start) // Abbruch bedingung 
	 {	  new_step_delay = srd.min_delay;
		 
		if(step_count >= srd.decel_start)
		{	
			decl_trigger();
		}
	 }
 }
  

}

void sm_driver_StepOutput()
{
	
	if (srd.dir == CW){
		digitalWrite(dirpin, LOW);   // sets the LED on
		digitalWrite(steppin, LOW);   // sets the LED on
		if(srd.run_state != BACKLASH) stepper_posi++;
	}
	else{
		if(srd.run_state != BACKLASH) stepper_posi--;
		digitalWrite(dirpin, HIGH);   // sets the LED on
		digitalWrite(steppin, LOW);   // sets the LED on
	}
   delayMicroseconds(stepper_delay);
   digitalWrite(steppin, HIGH);   // sets the LED on

}

void B_sm_driver_StepOutput()
{
	
	if (encoderEXT_dir == CW){
		digitalWrite(dirpinB, HIGH);   // sets the LED on
		digitalWrite(steppinB, LOW);   // sets the LED on
                if ( backlashB_count < default_backlash_moveB)	
                {backlashB_count ++;}
                else {stepperB_posi++;}
		
	}
	else{
		
		digitalWrite(dirpinB, LOW);   // sets the LED on
		digitalWrite(steppinB, LOW);   // sets the LED on
                if ( backlashB_count > 0)	
                {backlashB_count --;}
                else {stepperB_posi--;}
	}
   delayMicroseconds(stepper_delay);
   digitalWrite(steppinB, HIGH);   // sets the LED on

}



void speed_cntr_Move(signed long step, unsigned int speed)
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
	 status.backlash = TRUE;		 	
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
		if( srd.step_delay <= srd.min_delay){
			/*srd.step_delay = srd.min_delay;
			srd.run_state = RUN;*/
			
			if (status.backlash_trigger == TRUE)
			{ srd.run_state = BACKLASH;
			}
			
			else srd.run_state = ACCEL;
		}
		else{
			if (status.backlash_trigger == TRUE)
			{ srd.run_state = BACKLASH;
			} 
			
			else srd.run_state = ACCEL;
			
			}
					
		
		if (status.thread == FALSE ||status.backlash_trigger == TRUE)
		{		
		// Reset counter.
		srd.accel_count = 0;						
		status.running = TRUE;
		OCR1A = 10;
		// Set Timer/Counter to divide clock by 8
		TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));
		}
		
		
		
	}
	

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

	
	static unsigned int backlash_count = 0;	
	static unsigned int backlash_count_stepdelay = 0;
	// Counting steps when moving.
	// Keep track of remainder from new_step-delay calculation to incrase accurancy
	static unsigned int rest = 0;
	static unsigned int rest_temp = 0;
	
	OCR1A = srd.step_delay;

	switch(srd.run_state) {
		case STOP:	
        
		fehler = resolution/2;	
		step_count = 0;
		rest = 0;
		// Stop Timer/Counter 1.
		TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));
		if(status.thread == FALSE)status.running = FALSE;
		
		
						
		break;
		
		case ACCEL_DUMMY:
		if (spindle_flag == 1)
		{	spindle_flag = 2;
			spindle_posi = 0;
			step_count_temp = step_count;
			srd_temp.accel_count = srd.accel_count;
			new_step_delay_temp=new_step_delay;
						
		}		
		step_count++;
		srd.accel_count++;
		new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) + rest)/(4 * srd.accel_count + 1));
		rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
		
		// Check if we hitted max speed.
		if(new_step_delay <= srd.min_delay) {
			
						step_count = step_count_temp;
						srd.accel_count = srd_temp.accel_count;	
						new_step_delay = new_step_delay_temp;
						if (spindle_posi > resolution)
						{ spindle_start = resolution -( spindle_posi % resolution);
						}else spindle_start = resolution - spindle_posi;
						
						spindle_flag = 3;
						srd.run_state = STOP;
						
		}
		break;

		case ACCEL:
		if (spindle_flag == 1) {srd.run_state = ACCEL_DUMMY; }
		else {sm_driver_StepOutput();
		step_count++;
		srd.accel_count++;
		new_step_delay = srd.step_delay - (((2 * (long)srd.step_delay) + rest)/(4 * srd.accel_count + 1));
		rest = ((2 * (long)srd.step_delay)+rest)%(4 * srd.accel_count + 1);
		// Chech if we should start decelration.
		if(step_count >= srd.decel_start && mode == 0  && digitalRead (S1) && digitalRead (S2)&& digitalRead (S3) && digitalRead (S4))
		{
				srd.accel_count = srd.decel_val;
				srd.run_state = DECEL;
			
		}
		
		else if(step_count >= srd.decel_start && mode == 1 )
		{
			srd.accel_count = srd.decel_val;
			srd.run_state = DECEL;
			
		}
		// Check if we hitted max speed.
		else if(new_step_delay <= srd.min_delay) {
			last_accel_delay = new_step_delay;
			new_step_delay = srd.min_delay;
			rest = 0;
			if (status.thread == TRUE)
			{	TCCR1B &= ~((1<<CS12)|(1<<CS11)|(1<<CS10));
				srd.run_state = AUTO;
			status.thread = FALSE;
			 }else srd.run_state = RUN;
			}
			}
		break;
		
		case BACKLASH:
		sm_driver_StepOutput();
		backlash_count++;
		new_step_delay = backlash_speed;
		if (backlash_count >= configuration.backlash_move)
		{	
			
				  new_step_delay = (T1_FREQ_148 * sqrt(A_SQ/accel_stepper))/100;
                                  step_count = 0;
                                  rest = 0;
                                  backlash_count = 0;	
                                  srd.accel_count = 0;
                                                         
                                  status.backlash = FALSE;
                                  status.encoder_trigger =FALSE;	
                                  if(status.thread == TRUE) srd.run_state = ACCEL_DUMMY;
									 
								  else {srd.run_state = ACCEL;
                                
                                  OCR1A = 10;
                                  // Set Timer/Counter to divide clock by 8
                                  TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));}
			
					
		}
		break;
		
		
		case RUN:
		sm_driver_StepOutput();
		step_count++;
		new_step_delay = srd.min_delay;
		// Chech if we should start decelration.
		if(step_count >= srd.decel_start) 
		{
			if (mode == 0  && digitalRead (S1) && digitalRead (S2)&& digitalRead (S3) && digitalRead (S4))
			{
				srd.accel_count = srd.decel_val;
				// Start decelration with same delay as accel ended with.
				new_step_delay = last_accel_delay;
				srd.run_state = DECEL;
			}
			if (mode == 1)
			{
				srd.accel_count = srd.decel_val;
				// Start decelration with same delay as accel ended with.
				new_step_delay = last_accel_delay;
				srd.run_state = DECEL;
			}
			
			
		}

              else if (step_count >= srd.decel_start && mode != 0 ) 
		{
			srd.accel_count = srd.decel_val;
			// Start decelration with same delay as accel ended with.
			new_step_delay = last_accel_delay;
			srd.run_state = DECEL;
		}



		break;
		
		case AUTO: 
						
		break;

		case DECEL:
		sm_driver_StepOutput();
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

void io_init()
{
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

        pinMode(encoder_PinA, INPUT);
 	pinMode(encoder_PinB, INPUT);
        pinMode(encoderEXT_PinA, INPUT);
        pinMode(encoderEXT_PinB, INPUT);

	pinMode(spindle_PinA, INPUT);
	pinMode(spindle_PinB, INPUT);
        pinMode(spindle_PinZ, INPUT);
	
	
	pinMode(led1, OUTPUT);           // set pin to output
	pinMode(led2, OUTPUT);           // set pin to output
	pinMode(led3, OUTPUT);           // set pin to output
	pinMode(led4, OUTPUT);           // set pin to output
	pinMode(tweeter, OUTPUT);        // set pin to output
        pinMode(relais, OUTPUT);        // set pin to output

	pinMode(out3, OUTPUT);           // set pin to output
	pinMode(dirpin, OUTPUT);		 // set pin to output
	pinMode(steppin, OUTPUT);		 // set pin to output
        pinMode(dirpinB, OUTPUT);		 // set pin to output
	pinMode(steppinB, OUTPUT);		 // set pin to output
	
	digitalWrite(led1, HIGH);		 // sets the LED off
	digitalWrite(led2, HIGH);		 // sets the LED off
	digitalWrite(led3, HIGH);		 // sets the LED off
	digitalWrite(led4, HIGH);		 // sets the LED off
        digitalWrite(relais, HIGH);		 // sets the LED off
	
	digitalWrite(dirpin, HIGH);		 // sets the LED off
	digitalWrite(steppin, HIGH);     // sets the LED off
	
	digitalWrite(out3, HIGH);   // sets the LED off
	
	
}

long read_edit_number()
{
	if (menue == 0)
	{ if (edit == 1) return stepper_posi;
	  if (edit == 3) return configuration.thread_length;
	  if (edit == 4) return configuration.thread_pitch;	
	}
	if (menue == 1)
	{
		if (edit == 1) return stepper_posi;
		if (edit == 3) return configuration.grind_way;
		if (edit == 4) return configuration.grind_speed;
	}
	if (menue == 2)
	{
		if (edit == 1) return stepper_posi;
		if (edit == 3) return configuration.cutting_way;
		if (edit == 4) return configuration.cutting_speed;
	}
	if (menue == 3)
	{	if (edit == 1) return stepper_posi;
		if (edit == 2) return configuration.move_way;
		if (edit == 3) return configuration.move_fast_speed;
		if (edit == 4) return configuration.move_slow_speed;
	}
	if (menue == 4)
	{
	}
}

/*
edit = 

0 Zeile 1
1 Zeile 2
2 Zeile 3
3 Zeile 4
4 Zeile 5 

*/

void  trigger_edit_number(int value)
{	
	
	
	if (menue == 0)
	{	if (edit == 1)  stepper_posi += value* (steps_mm) ;
		if (edit == 3)  configuration.thread_length += value * (steps_mm/4);
		if (edit == 4)  configuration.thread_pitch += value * 5;
	}
	if (menue == 1)
	{
		if (edit == 1)  stepper_posi += value * (steps_mm);
		if (edit == 3)  configuration.grind_way += value* (steps_mm/4);
		if (edit == 4)  configuration. grind_speed += value*10;
	}
	if (menue == 2)
	{
		if (edit == 1)  stepper_posi += value * (steps_mm);
		if (edit == 3)  configuration.cutting_way += value* (steps_mm/4);
		if (edit == 4)  configuration.cutting_speed +=value*2;
	}
	if (menue == 3)
	{	if (edit == 1)  stepper_posi += value * (steps_mm);
		if (edit == 2)  configuration.move_way += value * (steps_mm/4);
		if (edit == 3)  configuration.move_fast_speed += value*10;
		if (edit == 4)  configuration.move_slow_speed += value*10;
	}
	if (menue == 4)
	{
		if (edit == 0)  configuration.delay_move += value*100;
		if (edit == 1)  configuration.fast_move += value*10;
		if (edit == 2)  configuration.slow_move += value*10;
		if (edit == 3)  configuration.backlash_move += value;
	}
}

void  write_edit_number(int value)
{
	
	
	if (menue == 0)
	{	if (edit == 1)  stepper_posi = value;
                if (edit == 2)  stepperB_posi = value;
		if (edit == 3)  configuration.thread_length = value;
		if (edit == 4)  configuration.thread_pitch = value;
	}
	if (menue == 1)
	{
		if (edit == 1)  stepper_posi = value ;
		if (edit == 3)  configuration.grind_way = value;
		if (edit == 4)  configuration.grind_speed = value ;
	}
	if (menue == 2)
	{
		if (edit == 1)  stepper_posi = value ;
		if (edit == 3)  configuration.cutting_way = value;
		if (edit == 4)  configuration.cutting_speed = value;
	}
	if (menue == 3)
	{	if (edit == 1)  stepper_posi = value ;
		if (edit == 2)  configuration.move_way = value;
		if (edit == 3)  configuration.move_fast_speed = value;
		if (edit == 4)  configuration.move_slow_speed = value;
	}
	if (menue == 4)
	{
		if (edit == 0)  configuration.delay_move = value;
		if (edit == 1)  configuration.fast_move = value;
		if (edit == 2)  configuration.slow_move = value;
		if (edit == 3)  configuration.backlash_move = value;
	}
}

void write_default_config()
{
 configuration.thread_pitch =default_thread_pitch;  // Steigung 2mm entspricht 200, 1.06 entspricht 106... !!
 configuration.thread_length =default_thread_length; 
 configuration.grind_way =default_grind_way ;
 configuration.grind_speed =default_grind_speed ;
 configuration.cutting_way =default_cutting_way ;
 configuration.cutting_speed =default_cutting_speed; 
 configuration.move_way=default_move_way ;
 configuration.move_fast_speed=default_move_fast_speed ;
 configuration.move_slow_speed=default_move_slow_speed; 
configuration.fast_move =default_fast_move ;
 configuration.slow_move=default_slow_move;
 configuration.delay_move=default_delay_move ;
 configuration.backlash_move=default_backlash_move ;
}

void decl_trigger()
{	TCCR1B |= ((0<<CS12)|(1<<CS11)|(0<<CS10));
	srd.accel_count = srd.decel_val;
	// Start decelration with same delay as accel ended with.
	new_step_delay = last_accel_delay;
	srd.run_state = DECEL;
}

void doSpindleZ(){

    
  digitalWrite(led3, !digitalRead(led3));
  spindle_angle = 0;
 
}

void doEncoderEXTB(){
  encoderEXT_Bnew^encoderEXT_Aold ? encoderEXT_posi++:encoderEXT_posi--;
  encoderEXT_Bnew^encoderEXT_Aold ? encoderEXT_dir = CCW :encoderEXT_dir = CW ;
  encoderEXT_Aold=digitalRead(encoderEXT_PinA); 
  B_sm_driver_StepOutput();
  
}
void doEncoderEXTA(){
  encoderEXT_Bnew=digitalRead(encoderEXT_PinB);
  encoderEXT_Bnew^encoderEXT_Aold ? encoderEXT_posi++:encoderEXT_posi--;
  encoderEXT_Bnew^encoderEXT_Aold ? encoderEXT_dir = CCW :encoderEXT_dir = CW ;
  B_sm_driver_StepOutput();
}
