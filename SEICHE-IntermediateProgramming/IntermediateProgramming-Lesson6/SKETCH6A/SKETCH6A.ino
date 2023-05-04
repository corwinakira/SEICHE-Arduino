#include <ESP8266WiFiMulti.h>
#include <time.h>
#include <SPI.h>
#include <MD_MAX72xx.h>
#include <MD_Parola.h>
#include"Parola-3x7.h"


// MD_Parola Setup
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES  4
#define CLK_PIN   14  // or SCK
#define DATA_PIN  13  // or MOSI
#define CS_PIN    15  // or SS
// MD_MAX72XX ledmx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
MD_Parola pmx = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// We always wait a bit between updates of the display
#define  DELAYTIME  100  // in milliseconds


// Setup for wifiMulti
ESP8266WiFiMulti wifiMulti;
// WiFi connect timeout per AP. Increase when connecting takes longer.
const uint32_t connectTimeoutMs = 5000;
char ipaddrbuffer[20];


void setup(){
  Serial.begin(9600);

// LED Matrix Setup
  pmx.begin();
  pmx.setZoneEffect(0, true, PA_FLIP_LR);
  pmx.setZoneEffect(0, true, PA_FLIP_UD); 
  pmx.setFont(0,parola_3x7);

  // Don't save WiFi configuration in flash - optional
  WiFi.persistent(false);

  Serial.println("\nESP8266 Initializing Multi WiFi\n");

  // Set WiFi to station mode
  WiFi.mode(WIFI_STA);

  // Register multi WiFi networks
  wifiMulti.addAP("CFC_Public", "");
  wifiMulti.addAP("PALAS", "xxxxxx");
  // wifiMulti.addAP("ssid_from_AP_3", "your_password_for_AP_3");

  if (wifiMulti.run(connectTimeoutMs) == WL_CONNECTED) 
  {
    Serial.println("WiFi connected!");
    Serial.print("WiFi SSID: ");
    Serial.println(WiFi.SSID());
    sprintf(ipaddrbuffer,"%s",WiFi.localIP().toString().c_str());
    Serial.print("WiFi IP Address - ipaddrbuffer: ");
    Serial.println(ipaddrbuffer);
  } 
  else 
  {
    Serial.println("WiFi not connected!");
  }
  Serial.println("\n\n");
}

void loop() 
{
    Serial.print("WiFi IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Sprintf IP Address: ");
    Serial.println(ipaddrbuffer);
    if(pmx.displayAnimate())
    {
      pmx.displayText(ipaddrbuffer,PA_CENTER,50,0,PA_SCROLL_LEFT,PA_SCROLL_LEFT);
      yield();
    }
    delay(1000);
}
