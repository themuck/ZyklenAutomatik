// include the library header
#include <glcd.h>

// include the Fonts
#include <fonts/allFonts.h>

#define encoder1PinA 19
#define encoder1PinB 20

#define encoder0PinA 2
#define encoder0PinB 3

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
/*
#define out3  8
#define out2  9
#define out1  10
*/

#define tweeter  32

#define resolution 80000 // Festkommaarithmetik Resolution * 100
#define fehler_def 40000 // Fehler = resolution/2
#define CCW 0
#define CW 1
#define TRUE 1
#define FALSE 0
#define steps_mm 50 // aus der Rechnungstabelle
#define steps_max_acel 20000 //steps/mm^2

#define dirpin  9 //pin fÃ¼r Richtung
#define steppin  10 //pin fÃ¼r Schritt

#define spindel_steps_help (steps_max_acel/steps_mm)