#include <SPI.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include"Parola-boldFont.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES  4
#define CLK_PIN   14  // or SCK
#define DATA_PIN  13  // or MOSI
#define CS_PIN    15  // or SS

// MD_MAX72XX ledmx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_Parola pmx = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// We always wait a bit between updates of the display
#define  DELAYTIME  100  // in milliseconds

void setup() 
{
  // put your setup code here, to run once:
  pmx.begin(2);
  pmx.setZone(0,0,1);
  pmx.setZone(1,2,3);
  pmx.setZoneEffect(0, true, PA_FLIP_LR);
  pmx.setZoneEffect(0, true, PA_FLIP_UD);
  pmx.setZoneEffect(1, true, PA_FLIP_LR);
  pmx.setZoneEffect(1, true, PA_FLIP_UD);
  pmx.setFont(1,parola_boldFont);
}

void loop() 
{
  if(pmx.displayAnimate())
  {
    pmx.displayZoneText(0,"SEICHE",PA_CENTER,50,0,PA_SCROLL_RIGHT,PA_SCROLL_RIGHT);
    pmx.displayZoneText(1,"2022",PA_CENTER,50,0,PA_SCROLL_LEFT,PA_SCROLL_LEFT);
  }
}
