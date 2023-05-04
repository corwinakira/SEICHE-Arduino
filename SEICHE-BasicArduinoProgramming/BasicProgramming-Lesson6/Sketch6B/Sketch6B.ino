#include <MD_MAX72xx.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES  4
#define CLK_PIN   14  // or SCK
#define DATA_PIN  13  // or MOSI
#define CS_PIN    15  // or SS

MD_MAX72XX ledmx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// We always wait a bit between updates of the display
#define  DELAYTIME  100  // in milliseconds

void setup() {
  // put your setup code here, to run once:
  ledmx.begin();
  ledmx.control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
//  ledmx.clear();
  Serial.begin(9600);
  Serial.println("MAX7219 TEST");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(DELAYTIME);
  ledmx.setPoint(5,5,true);
  delay(DELAYTIME+500);
  ledmx.setPoint(5,5,false);
  delay(DELAYTIME+500);
}
