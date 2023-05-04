#include <SPI.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <Adafruit_BMP085.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES  4
#define CLK_PIN   14  // or SCK
#define DATA_PIN  13  // or MOSI
#define CS_PIN    15  // or SS

// MD_MAX72XX ledmx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_Parola pmx = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Setup for BMP085 or BMP180
Adafruit_BMP085 bmp; // I2C
static int pmb = 888;

// We always wait a bit between updates of the display
#define  DELAYTIME  100  // in milliseconds

long elapsed_time;

void setup() 
{
  Serial.begin(9600);
  
  // put your setup code here, to run once:
  pmx.begin();
  pmx.setZoneEffect(0, true, PA_FLIP_LR);
  pmx.setZoneEffect(0, true, PA_FLIP_UD);

  // BMP180 ACCESS - Will Crash If Sensor Not Found - Annoying!!
  Serial.println();
  Serial.println("BMP180 initialization");

  if (!bmp.begin()) 
  {
    Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    abort();
  }
  else
  {
// Confirm BMP180 sensor is working
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" C");
    pmb = bmp.readPressure()/100;                                                 
  }
  elapsed_time=millis();
}

void loop() 
{
  Serial.println(elapsed_time,DEC);
  if(pmx.displayAnimate())
    pmx.displayText("SEICHE",PA_CENTER,50,0,PA_PRINT,PA_NO_EFFECT);
  delay(DELAYTIME);
  Serial.print("Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" C");
  delay(5000);
  elapsed_time=millis();
}
