#include <SPI.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES  4
#define CLK_PIN   14  // or SCK
#define DATA_PIN  13  // or MOSI
#define CS_PIN    15  // or SS

// MD_MAX72XX ledmx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_Parola pmx = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// We always wait a bit between updates of the display
#define  DELAYTIME  100  // in milliseconds

// sprintf test variables
int i = 65536;
float flt = 99888777.66;
byte b=255;
double df=2.2250738585072014e-308;
long l=9223372036854775807;

void setup() 
{
  // put your setup code here, to run once:
  pmx.begin();
  pmx.setZoneEffect(0, true, PA_FLIP_LR);
  pmx.setZoneEffect(0, true, PA_FLIP_UD);
}

void loop() 
{
  char chrbuff[32];
  sprintf(chrbuff,"%s","SEICHE 2022");
  if(pmx.displayAnimate())
    pmx.displayText(chrbuff,PA_LEFT,50,0,PA_SCROLL_LEFT,PA_SCROLL_LEFT);
}
