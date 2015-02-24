/* 
	Editor: http://www.visualmicro.com
	        arduino debugger, visual micro +, free forum and wiki
	
	Hardware: Arduino Mega 2560 or Mega ADK, Platform=avr, Package=arduino
*/

/*
#define __AVR_ATmega2560__
#define ARDUINO 101
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define __cplusplus
extern "C" void __cxa_pure_virtual() {;}
*/
//
//
void print_menue ();
void print_edit_cursor ();
void print_menue_numbers ();
void doEncoderB();
void doSpindleA();
void doSpindleB();
void sm_driver_StepOutput();
void B_sm_driver_StepOutput();
void speed_cntr_Move(signed long step, unsigned int speed);
void speed_cntr_Init_Timer1(void);
void io_init();
long read_edit_number();
void  trigger_edit_number(int value);
void  write_edit_number(int value);
void write_default_config();
void decl_trigger();
void doSpindleZ();
void doEncoderEXTB();
void doEncoderEXTA();
/*
#include "C:\Program Files (x86)\Arduino\hardware\arduino\variants\mega\pins_arduino.h" 
#include "C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Users\Malte\SkyDrive\ZyklenAutomatik\Zyklenautomatik\Zyklenautomatik.ino"
#include "C:\Users\Malte\SkyDrive\ZyklenAutomatik\Zyklenautomatik\EEPROMAnything.h"
#include "C:\Users\Malte\SkyDrive\ZyklenAutomatik\Zyklenautomatik\ZyklenAutomatik_Config.h"
*/
