char c;
void setup() 
{
  // Setup port for serial monitor
  Serial.begin(9600);
}

// Still printing the ASCII character set!
void loop() 
{
  c = 0;
  while(c < 256)
  {
     Serial.print("ASCII: "); Serial.println(c);      
     Serial.print ("DEC: "); Serial.println(c, DEC);    
     c++;     
     delay(500);
  }
}
