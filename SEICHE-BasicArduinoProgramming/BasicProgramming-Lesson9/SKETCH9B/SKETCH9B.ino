int avalue;
void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  avalue = analogRead(A0);
  Serial.println(avalue,DEC);
  delay(250);
}
