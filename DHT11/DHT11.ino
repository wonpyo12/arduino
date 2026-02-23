#include <EduIntro.h>

DHT11 dht11(A0);  // creating the object sensor on pin 'D7'

int C;   // temperature C readings are integers

int H;   // humidity readings are integers

void setup()
{
  // initialize serial communications at 9600 bps
  Serial.begin(9600);
}

void loop()
{
  dht11.update();

  C = dht11.readCelsius();     	// Reading the temperature in Celsius degrees and store in the C variable
 
  H = dht11.readHumidity();     // Reading the humidity index

 
  
  Serial.print(H);
  
  Serial.print(",");
  
  Serial.println(C);
 
  delay(2000);                // Wait one second before get another temperature reading
}
