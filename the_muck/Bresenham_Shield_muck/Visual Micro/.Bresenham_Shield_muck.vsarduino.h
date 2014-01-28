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
void print_menue (int menue);
void print_edit (int edit);
void print_menue_numbers (int menue);
void doPotiB();
void doEncoderA();
void doEncoderB();
void sm_driver_StepCounter(signed char inc);
void sm_driver_StepOutput();
void speed_cntr_Move(signed int step, unsigned int speed);
void speed_cntr_Init_Timer1(void);

#include "C:\Program Files (x86)\Arduino\hardware\arduino\variants\mega\pins_arduino.h" 
#include "C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck\Bresenham_Shield_muck.ino"
#include "C:\Users\Malte\Documents\GitHub\ZyklenAutomatik\the_muck\Bresenham_Shield_muck\ZyklenAutomatik_Config.h"
