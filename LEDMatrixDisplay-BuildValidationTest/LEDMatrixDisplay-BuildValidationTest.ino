/* #include <ESP8266WiFi.h> */
#include <ESP8266WiFiMulti.h>
#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <ezTime.h>
#include <Wire.h>
#include <time.h>
#include <Adafruit_BMP085.h>
#include <RTClib.h>

#define TIMEDEBUG 0
#define SERTIME 1
#define SENSORDEBUG 0
#define RTCDEBUG 1

#define note_F2 87.31                                                                                               
#define note_C3 130.81                                                                                              
#define note_C3s 138.59                                                                                             
#define note_D3 146.83                                                                                              
#define note_D3s 155.56                                                                                             
#define note_E3 164.81                                                                                              
#define note_F3 174.61                                                                                              
#define note_F3s 185                                                                                                
#define note_G3 196                                                                                                 
#define note_G3s 207.65                                                                                             
#define note_A3 220                                                                                                 
#define note_A3s 233.08                                                                                             
#define note_B3 246.94                                                                                              
#define note_C4 261.63                                                                                              
#define note_C4s 277.18                                                                                             
#define note_D4 293.66                                                                                              
#define note_D4s 311.13                                                                                             
#define note_E4 329.63                                                                                              
#define note_F4 349.23                                                                                              
#define note F4s 369.99                                                                                             
#define note_G4 392                                                                                                 
#define note_G4s 415.30                                                                                             
#define note_A4f 415.30                                                                                             
#define note_A4 440
#define note_A4s 466.16
#define note_B4f 466.16
#define note_B4 493.88
#define note_C5 523.25
#define note_C5s 554.37
#define note_D5s 622.25
#define note_D5 587.33
#define note_E5 659.25
#define note_F5 698.46
#define note_F5s 739.99
#define note_G5 783.99

#define BUTTON D3

// Setup for U8G2 using MAX7219 with GPIO14_SCLKt=CLK, GPIO13_MOSI=DIN, GPIO15_CS=CS
U8G2_MAX7219_32X8_F_4W_SW_SPI u8g2(U8G2_R2, /* clock=*/ 14, /* data=*/ 13, /* cs=*/ 15, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);

// Setup for wifiMulti
ESP8266WiFiMulti wifiMulti;
// WiFi connect timeout per AP. Increase when connecting takes longer.
const uint32_t connectTimeoutMs = 5000;

// Setup for EZ Time

Timezone myTZ;
Timezone loopTZ;

// Setup for BMP280
Adafruit_BMP085 bmp; // I2C

//Setup for DS3231 RTC
RTC_DS3231 rtc;

// Local sketch setup
static int tzsetFlag = 0;
static uint8_t hours = 0;
static uint8_t minutes = 0, mintens = 0, minones = 0;
static int a0value = 999, calcbright = 64, pmb = 888;
static bool buttonflag = 0;
static uint8_t findex=0, ofindex=0;

int decyear, decmonth, decday, dechr, decmin, decsec;
int rtcyear, rtcmonth, rtcday, rtchr, rtcmin, rtcsec;
bool soundflag;

void setup(void) 
{
  pinMode(LED_BUILTIN, OUTPUT);

  attachInterrupt(BUTTON,button_isr,FALLING);

  Serial.begin(9600);
  u8g2.begin();
  u8g2.setContrast(calcbright);

// u8g2_font_u8glib_4_tf
// u8g2_font_5x7_tf
  u8g2.setFont(u8g2_font_courB08_tf);
  u8g2.setDrawColor(1);
  
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setCursor(0,7);
  u8g2.print("INIT");
  u8g2.sendBuffer();
  delay(750);

  a0value=analogRead(A0);
  calcbright = a0value/8;
  Serial.println();
  Serial.print("A0: ");
  Serial.println(a0value, DEC);
  Serial.print("Brightness: ");
  Serial.println(calcbright, DEC);

  u8g2.setContrast(calcbright);
  u8g2.setFont(u8g2_font_squeezed_r7_tr);
  u8g2.clearBuffer();          // clear the internal memory
  u8g2.setCursor(0,7);
  u8g2.print("A0:");
// u8g2.setCursor(16,7);
  u8g2.print(u8x8_u16toa(a0value,4));
  u8g2.sendBuffer();

  delay(1000);

  // Don't save WiFi configuration in flash - optional
  WiFi.persistent(false);

  Serial.println("\nESP8266 Initializing Multi WiFi\n");

  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);

  // Register multi WiFi networks
  wifiMulti.addAP("CFC_Public", "");
  wifiMulti.addAP("PALAS", "abacab00");
  // wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) 
  {
    Serial.println("WiFi connected!");
    Serial.print("WiFi SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("WiFi IP Address: ");
    Serial.println(WiFi.localIP());
    u8g2.setFont(u8g2_font_squeezed_r7_tr);
    u8g2.clear();
    u8g2.setCursor(0,7);
    u8g2.print(WiFi.SSID());
    u8g2.sendBuffer();
    delay(1000);
    u8g2.clear();
    u8g2.setCursor(0,7);
    u8g2.print(WiFi.localIP());
    u8g2.sendBuffer();
    tone(D6,220,1000);
    delay(250);
    noTone(D6);
    delay(1500);
  } 
  else 
  {
    Serial.println("WiFi not connected!");
    u8g2.clear();
    u8g2.setCursor(0,7);
    u8g2.print("NO WIFI");
    u8g2.sendBuffer();
 //   abort();
  }

  // This is wait for NTP sync-- EZtime library
  Serial.println("NTP sync initiated");
  waitForSync();

  Serial.println();
  Serial.println("UTC:   " + UTC.dateTime());

  setInterval(900); // NTP update interval set to 15 minutes

  // Provide official timezone names
  // https://en.wikipedia.org/wiki/List_of_tz_database_time_zones
  if(! myTZ.setCache(0))
  {
    myTZ.setLocation(F("America/Indiana/Indianapolis"));
    myTZ.setCache(0);
  }
  Serial.println("Indianapolis:     " + myTZ.dateTime());
  // Show NTP time acquired during startup
  u8g2.clear();
  u8g2.setCursor(0,7);
  u8g2.print("N#");
  u8g2.print(myTZ.hour());
  u8g2.print(":");
  u8g2.print(myTZ.minute());
  u8g2.sendBuffer();
  tone(D6,440,1000);
  delay(250);
  noTone(D6);
  delay(3000);

// This assures that DST was actually set
  if (TIMEDEBUG)
  {
    Serial.println(now());
    Serial.println(dateTime(COOKIE));
    Serial.println(isDST()?"Summer":"Winter");
  }
  myTZ.setDefault();
  if (TIMEDEBUG)
  {
    Serial.println(now());
    Serial.println(dateTime(COOKIE));
    Serial.println(isDST()?"Summer":"Winter");
  }
  decyear = UTC.year();
  decmonth = UTC.month();
  decday = UTC.day();
  dechr = UTC.hour();
  decmin = UTC.minute();
  decsec = UTC.second();

// BEGIN RTC CHECK
  u8g2.setFont(u8g2_font_courB08_tr);
  u8g2.setDrawColor(1);
  u8g2.clear();          // clear the internal memory

  //RTC setup - display only for now
  if (! rtc.begin()) 
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    u8g2.setCursor(0,7);
    u8g2.print("NORTC");
    u8g2.sendBuffer();
    delay(2000);
 //   abort();
  }
  else
  {
    Serial.print("RTC Temperature: ");
    Serial.print(rtc.getTemperature());
    Serial.println(" C");

    u8g2.setCursor(4,7);
    u8g2.print(u8x8_u8toa(rtc.getTemperature(),3));
    u8g2.print("C");
    u8g2.sendBuffer();
    tone(D6,660,1000);
    delay(500);
    noTone(D6);
    delay(1500);
// FRom the example, to set the RTC with an explicit date & time
// For instance, for January 21, 2014 at 3am you would call:
// Use: rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    if (rtc.lostPower()) 
    {
      Serial.println("RTC lost power!");
      u8g2.clear(); 
      u8g2.setCursor(2,7);
      u8g2.print("RTCLS");
      u8g2.sendBuffer();
      tone(D6,660,1000);
      delay(500);
      noTone(D6);
      delay(1500);
//      DateTime tmpdt;
//      rtc.adjust(DateTime("Jan 22 2014","3:0:0"));
/*      rtc.adjust(DateTime(decyear, decmonth, decday, dechr, decmin, decsec));
      Serial.println("RTC date and time adjusted");
*/
    }
  }

// BMP180 CHECK - Will Crash If Sensor Not Found - Annoying!!
  Serial.println();
  Serial.println("Start BMP180 initialization");

  if (!bmp.begin()) 
  {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    abort();
  }
  else
  {
// Confirm BMP280 sensor is working
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" C");
    pmb = bmp.readPressure()/100;
    Serial.print("Pressure = ");
    Serial.print(pmb,DEC);
    Serial.println(" mBar");   
    u8g2.clear();          // clear the internal memory
    u8g2.setFont(u8g2_font_squeezed_r7_tr);
    u8g2.setCursor(0,7);
    u8g2.print(u8x8_u16toa(pmb,4));
    u8g2.setCursor(22,7);
    u8g2.print("mB");
    u8g2.sendBuffer();
    tone(D6,880,1000);
    delay(500);
    noTone(D6);
    delay(1500);
    u8g2.setFont(u8g2_font_courB08_tr);
    u8g2.setDrawColor(1);
    u8g2.clear();          // clear the internal memory
    u8g2.setCursor(4,7);
    u8g2.print(u8x8_u8toa(bmp.readTemperature()*1.8+32,3));
  //  u8g2.setCursor(20,7);
    u8g2.print("F");
    u8g2.sendBuffer();  
    tone(D6,880,1000);
    delay(500);
    noTone(D6);
    delay(1500);                                                 
  }

// ALL CHECKS PASSED
  u8g2.clear();
  u8g2.setFont(u8g2_font_courB08_tf);
  u8g2.setCursor(0,7);
  u8g2.print("READY");
  u8g2.sendBuffer();

  tone(D6,note_G4,2000);
  delay(250);
  tone(D6,note_A4,2000);
  delay(250);
  tone(D6,note_F4,2000);
  delay(250);
  tone(D6,note_F3,2000);
  delay(250);
  tone(D6,note_C4,2000);
  delay(750);
  noTone(D6);
}

/********************************************
 ****LOOPLOOPLOOPLOOPLOOPLOOPLOOPLOOPLOOP****
 ********************************************/

void loop(void)
{
  int k, kdigit, cdigit, ddigit, odigit;

  
  events();

  if (ofindex != findex)
  {
    Serial.print("Findex changed to ");
    Serial.println(findex,DEC);
    buttonflag=1;
    ofindex=findex;
    tone(D6,note_C3,250);
    delay(250);
    tone(D6,note_C4,250);
    delay(250);
    noTone(D6);
  }

  if(decsec%10 == 0) { Serial.println("..."); }

// Begin time processing and display
// Sometimes EZtime loses track of the timezone on ESP8266/ESP32
// so sort out timezone instead of displaying time if/when that happens
  if (!tzsetFlag)
  {
    if(!myTZ.setCache(0))
    {
      if(!myTZ.setLocation(F("America/Indiana/Indianapolis")))
      {
        if (TIMEDEBUG) { Serial.println("Received error message, retrying TZ set"); }
      }
      else
      {
        myTZ.setCache(0); 
      }
    }
    else
    {
      if (TIMEDEBUG) { Serial.println("Indianapolis TZ set"); }
      tzsetFlag = 1;
      if (TIMEDEBUG) { Serial.print("TZ set flag now: "); }
      if (TIMEDEBUG) { Serial.println(tzsetFlag); }
    } 
  }
  else
  {
    dechr = myTZ.hour();
    decmin = myTZ.minute();
    decsec = myTZ.second();

    // Only print time to serial port every 10 seconds; reduces clutter
    if (SERTIME && (decsec%10 == 0)) { Serial.print("myTZ DT:  "); }
    if (SERTIME && (decsec%10 == 0)) { Serial.println(myTZ.dateTime(COOKIE)); }
    if (TIMEDEBUG) { Serial.print("Decimal hour: "); }
    if (TIMEDEBUG) { Serial.println(dechr, DEC); }
    if (TIMEDEBUG) { Serial.print("Decimal minute: "); }
    if (TIMEDEBUG) { Serial.println(decmin, DEC); }
    if (TIMEDEBUG) { Serial.print("Decimal seconds: "); }
    if (TIMEDEBUG) { Serial.println(decsec, DEC); }

    if (decmin == 0)
    {
      ddigit = 0;
      odigit = 0;
    }
    else
    {
      odigit = decmin % 10;
      ddigit = (decmin - odigit)/10;
    }

    a0value=analogRead(A0);
    calcbright = a0value/8;
    u8g2.setContrast(calcbright);
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_courB08_tr); // choose a suitable font
    u8g2.setDrawColor(1);

// Insert findex selection block here for NTP time display

    if (findex == 0 || findex == 1)
    {
  // Actually display the time
      u8g2.setCursor(2,7);
      u8g2.print(u8x8_u8toa(dechr,2));
      u8g2.setCursor(18,7);
      u8g2.print(u8x8_u8toa(ddigit,1));
      u8g2.setCursor(24,7);
      u8g2.print(u8x8_u8toa(odigit,1));
  // Display the colon
      u8g2.drawPixel(15,2);
      u8g2.drawPixel(15,5);
      u8g2.sendBuffer();
      if (findex == 0 || findex == 5)
      {
        delay(750);
      }
      else if (findex == 1)
      {
        delay (500);
        // Clear the colon by using "black" color zero
        u8g2.setDrawColor(0);
        u8g2.drawPixel(15,2);
        u8g2.drawPixel(15,5);
        u8g2.sendBuffer();
        delay (500);
      }
    }
  // End NTP time processing and display
  }

  // Check for periodic alarm
  if (decmin%15==0 && decsec<10 && soundflag)
  {
    tone(D6,note_G4,2000);
    delay(500);
    tone(D6,note_C4,2000);
    delay(500);
    noTone(D6);
    soundflag=0;
  }
  else if (decmin%15!=0 && soundflag==0)
  {
    soundflag=1;
  }
  
// Turn the LED on for blinking
// Note that LOW is correct for illumination
// This is because the LED is active low on the ESP-01/ESP8266)
  digitalWrite(LED_BUILTIN, LOW);
  delay(250);

// Just some notes on fonts to try to fit the various
// values onto the LED matrix display all at once
// u8g2_font_smallsimple_tr - really tiny!
// u8g2_font_squeezed_b7_tr
// u8g2_font_courB08_tr - Original A0 font
// u8g2_font_squeezed_r7_tr - Narrow, for mBar and large A0
// u8g2_font_pxplusibmcga_8n - Fat, original primary time display font
 
  a0value=analogRead(A0);
  calcbright = a0value/8;
  u8g2.setContrast(calcbright);
  u8g2.setFont(u8g2_font_squeezed_r7_tr);
  u8g2.setDrawColor(1);
  
  if (SENSORDEBUG)
  {
    Serial.print("A0: ");
    Serial.println(a0value, DEC);
  }

  //
  // ADC value
  //
  if (findex == 0 || findex == 2)
  {
    u8g2.clear();          // clear the internal memory
    u8g2.setCursor(0,7);
    u8g2.print("A0:");
 //   u8g2.setCursor(16,7);
    u8g2.print(u8x8_u16toa(a0value,4));
    u8g2.sendBuffer();
    delay(750);
  } 

//
// BMP180 Temperature
//
  if (SENSORDEBUG)
  {
    Serial.print("Temperature = ");
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");
  }

  if (findex == 0 || findex == 3)
  {
    u8g2.setFont(u8g2_font_courB08_tr);
    u8g2.setDrawColor(1);
    u8g2.clear();          // clear the internal memory
    u8g2.setCursor(4,7);
    u8g2.print(u8x8_u8toa(bmp.readTemperature()*1.8+32,2));
    u8g2.setCursor(20,7);
    u8g2.print("F");
    u8g2.sendBuffer();
    delay(750);  
  }

//
// BMP180 Pressure
//
  pmb = bmp.readPressure()/100;
  if (SENSORDEBUG)
  {
    Serial.print("Pressure = ");
    Serial.print(pmb,DEC);
    Serial.println(" mBar");
  }

  if (findex == 0 || findex == 4)
  {
    u8g2.clear();          // clear the internal memory
    u8g2.setFont(u8g2_font_squeezed_r7_tr);
    u8g2.setCursor(0,7);
    u8g2.print(u8x8_u16toa(pmb,4));
    u8g2.setCursor(22,7);
    u8g2.print("mB");
    u8g2.sendBuffer();
    delay(750);
  }

//
// RTC time display
//
  if (findex == 0 || findex == 5)
  {
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

  // Actually display the time
    u8g2.clear();
    u8g2.setFont(u8g2_font_smallsimple_tr);
    u8g2.setDrawColor(1);
    u8g2.setCursor(0,7);
    u8g2.print("R:");
    u8g2.setCursor(8,7);
    u8g2.print(u8x8_u8toa(rtchr,2));
    u8g2.setCursor(20,7);
    u8g2.print(u8x8_u8toa(ddigit,1));
    u8g2.setCursor(26,7);
    u8g2.print(u8x8_u8toa(odigit,1));
  // Display the colon
    u8g2.drawPixel(17,2);
    u8g2.drawPixel(17,5);
    u8g2.sendBuffer();
    if (findex == 0)
    {
      delay(750);
    }
    else if (findex == 5)
    {
      delay (500);
      // Clear the colon by using "black" color zero
      u8g2.setDrawColor(0);
      u8g2.drawPixel(17,2);
      u8g2.drawPixel(17,5);
      u8g2.sendBuffer();
      delay (500);
    }
  }
  // Turn the LED off
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  delay(250);  

  yield();
}

ICACHE_RAM_ATTR void button_isr()
{
  Serial.println("\nButton pressed!");
  Serial.print("Findex was:"); Serial.print(findex,DEC); Serial.print("\n");
  findex++;
  if (findex > 5) { findex = 0; }
  Serial.print("Findex is:"); Serial.print(findex,DEC); Serial.print("\n\n");
}
