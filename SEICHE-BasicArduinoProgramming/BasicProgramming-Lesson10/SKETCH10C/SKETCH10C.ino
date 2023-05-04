#include <SPI.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include <Adafruit_BMP085.h>
#include <RTClib.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES  4
#define CLK_PIN   14  // or SCK
#define DATA_PIN  13  // or MOSI
#define CS_PIN    15  // or SS

//Setup for DS3231 RTC
RTC_DS3231 rtc;
int rtcyear, rtcmonth, rtcday, rtchr, rtcmin, rtcsec;
#define RTCDEBUG true

// MD_MAX72XX ledmx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_Parola pmx = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

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

//RTC setup - display only for now
  if (! rtc.begin()) 
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
 //   abort();
  }
  else
  {
    Serial.print("RTC Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");
  }
  if (rtc.lostPower()) 
  {
    Serial.println("RTC lost power!");
  }
  elapsed_time=millis();
}

void loop() 
{
  char chrbuff[5];
  int ddigit, odigit;
  Serial.println(elapsed_time,DEC);
  DateTime now = rtc.now();
  rtchr = now.hour();
  rtcmin = now.minute();
  rtcsec = now.second();
  if (rtcmin == 0)
  {
    ddigit = 0;
    odigit = 0;
  }
  else
  {
    odigit = rtcmin % 10;
    ddigit = (rtcmin - odigit)/10;
  }
  if (RTCDEBUG) { Serial.print("Decimal hour: "); }
  if (RTCDEBUG) { Serial.println(rtchr, DEC); }
  if (RTCDEBUG) { Serial.print("Decimal minute: "); }
  if (RTCDEBUG) { Serial.println(rtcmin, DEC); }
  if (RTCDEBUG) { Serial.print("Decimal seconds: "); }
  if (RTCDEBUG) { Serial.println(rtcsec, DEC); }
  sprintf(chrbuff,"%d:%d",rtchr,rtcmin);
  if(pmx.displayAnimate())
    pmx.displayText(chrbuff,PA_CENTER,50,0,PA_PRINT,PA_NO_EFFECT);
  delay(DELAYTIME);
  elapsed_time=millis();
}
