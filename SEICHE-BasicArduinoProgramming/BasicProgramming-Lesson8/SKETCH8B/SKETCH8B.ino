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

void setup() {
  // put your setup code here, to run once:
    tone(D6,220,1000);
    delay(2000);
    noTone(D6);
    delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
    tone(D6,880,1000);
    delay(500);
    noTone(D6);
    delay(1000);
}
