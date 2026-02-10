int moter = 6;
int motercon = 7;
void setup() {
  pinMode(moter, OUTPUT);
  pinMode(motercon, OUTPUT);

}

void loop() {
  digitalWrite(moter,HIGH);
  digitalWrite(motercon, LOW);
  delay(1000);


  digitalWrite(moter,LOW);
  digitalWrite(motercon, LOW);
  delay(1000);
}
