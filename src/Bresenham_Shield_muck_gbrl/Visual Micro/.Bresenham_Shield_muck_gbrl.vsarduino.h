/* 
	Editor: http://www.visualmicro.com
	        arduino debugger, visual micro +, free forum and wiki
	
	Hardware: Arduino Mega 2560 or Mega ADK, Platform=avr, Package=arduino
*/

#define __AVR_ATmega2560__
#define ARDUINO 101
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define __cplusplus
extern "C" void __cxa_pure_virtual() {;}

//
//
void print_menue ();
void print_edit_cursor ();
void print_menue_numbers ();
void doEncoderB();
void doSpindleA();
void doSpindleB();
void sm_driver_StepCounter();
void sm_driver_StepOutput();
void bresenham_StepOutput();
void speed_cntr_Move(signed int step, unsigned int speed);
void speed_cntr_Init_Timer1(void);
void io_init();
long read_edit_number();
void  trigger_edit_number(int value);
void  write_edit_number(int value);
void write_default_config();
void decl_trigger();

#include "C:\Program Files (x86)\Arduino\hardware\arduino\variants\mega\pins_arduino.h" 
#include "C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\Bresenham_Shield_muck_gbrl.ino"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\EEPROMAnything.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\ZyklenAutomatik_Config.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\config.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\coolant_control.c"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\coolant_control.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\eeprom.c"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\eeprom.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\gcode.c"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\gcode.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\limits.c"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\limits.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\main.c"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\motion_control.c"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\motion_control.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\nuts_bolts.c"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\nuts_bolts.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\planner.c"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\planner.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\print.c"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\print.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\protocol.c"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\protocol.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\serial.c"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\serial.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\settings.c"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\settings.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\spindle_control.c"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\spindle_control.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\stepper.c"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck_gbrl\stepper.h"
