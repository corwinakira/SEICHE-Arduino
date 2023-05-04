char aletter;

void setup() 
{
  // Setup port for serial monitor
  Serial.begin(9600);
}

// This time around, we will print the ASCII character set!
void loop() 
{
  Serial.println(aletter, DEC);
  Serial.println(aletter,DEC);
  Serial.println();

  aletter++; // Wait, what?!

// Leave delay at 500 or higher to have output be readable
  delay(500);
}
