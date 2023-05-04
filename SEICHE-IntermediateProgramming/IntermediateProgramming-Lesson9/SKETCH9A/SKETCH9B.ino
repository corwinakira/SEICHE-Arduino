/*********
  Original Source:
      Rui Santos
      Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-input-data-html-form/
  
      Permission is hereby granted, free of charge, to any person obtaining a copy
      of this software and associated documentation files.
      
      The above copyright notice and this permission notice shall be included in all
      copies or substantial portions of the Software.

      Notes
      - Recheck WiFi credentials for your network
      - This sketch does not use SPIFFS. So all the HTML is included in the sketch itself. That can get a little confusing!
*********/


#include <Arduino.h>

// Notice these platform-specific preprocessor directives!
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8
#define CS_PIN    15 // or SS
// HARDWARE SPI
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
// Scrolling parameters
uint8_t frameDelay = 25;  // default frame delay value
textEffect_t  scrollEffect = PA_SCROLL_LEFT;


// This _declares_ the webserver, it does not _start_ it.
AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS
const char* ssid = "CFC_Public";
const char* password = "";

const char* PARAM_INPUT_1 = "input1";

// Global message buffers shared by Wifi and Scrolling functions
#define BUF_SIZE  512
char curMessage[BUF_SIZE];
char newMessage[BUF_SIZE];
bool newMessageAvailable = false;

// HTML web page to handle 3 input fields (input1, input2, input3)
// SPIFFS is not used, so the page is included in the code
// Note the storage type of PROGMEM to cause the page to be stored in flash
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
</head>
<body>
  <form action="/get">
    input1: <input type="text" name="input1">
    <input type="submit" value="Submit">
  </form><br>
</body>
</html>)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

void setup() {
  Serial.begin(9600);
  Serial.println("Serial port initialized");
  Serial.println("Serial port initialized");
  Serial.println("Serial port initialized");
  Serial.println("Serial port initialized");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed!");
    return;
  }
  Serial.println();
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Send web page with input fields to client -- THIS starts the web server listening on port 80
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) 
  {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) 
    {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      newMessageAvailable = true;
      Serial.println("New message received");
      Serial.println(inputMessage);
      sprintf(curMessage,"%s\0",inputMessage);
    }
    else 
    {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                     + inputParam + ") with value: " + inputMessage +
                                     "<br><a href=\"/\">Return to Home Page</a>");
  });
  server.onNotFound(notFound);
  server.begin();

  P.begin();
  P.setZoneEffect(0, true, PA_FLIP_LR);
  P.setZoneEffect(0, true, PA_FLIP_UD); 
  P.setIntensity(0);
  P.displayClear();
  P.displaySuspend(false);
 
  P.displayScroll(curMessage, PA_LEFT, scrollEffect, frameDelay);
  curMessage[0] = newMessage[0] = '\0';
  sprintf(curMessage, "%03d:%03d:%03d:%03d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);
}

void loop() 
{
  if (P.displayAnimate())
  {
    if (newMessageAvailable)
    {
      Serial.print("New Message Is: ");
      Serial.println(curMessage);
//      strcpy(curMessage, newMessage);
//      sprintf(curMessage,"This is a test");
      P.displayScroll(curMessage, PA_LEFT, scrollEffect, frameDelay);
      newMessageAvailable = false;
    }
    P.displayReset();
  }
  
}
