#define BUTTON D3
int ivalue;
void setup() 
{
  Serial.begin(9600);
  pinMode(BUTTON,INPUT_PULLUP);
}

void loop() 
{
  ivalue = digitalRead(BUTTON);
  Serial.println(ivalue,DEC);
  delay(250);
}  
