#include <Servo.h>

Servo myservo;  // create servo object to control a servo

int potpin = A3;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin
int LEDPIN = 11;
int LEDSPIN =12;

void setup() {
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  pinMode(LEDPIN,OUTPUT );
  pinMode(LEDSPIN,OUTPUT );
  Serial.begin(9600);
}

void loop() {
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 1023, 0, 180); 
  if(val<30||val>150){
    digitalWrite(LEDPIN, HIGH);
    
    digitalWrite(LEDSPIN, LOW);
  }else{
    digitalWrite(LEDPIN, LOW);
    
    digitalWrite(LEDSPIN, HIGH);
  }    // scale it to use it with the servo (value between 0 and 180)
  
  myservo.write(val);                  // sets the servo position according to the scaled value
  Serial.println(val);
  delay(15);                           // waits for the servo to get there
}