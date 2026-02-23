#include <ESP8266WiFi.h>
#include <WiFiClient.h>

const char* ssid = "test";
const char* password = "12345678";

WiFiClient client;
const char* gost = "192.168.121.1";
unsigned int port = 12345;
char wbuf[20];

void setup(){
  Serial.begin(115200);
  delay(20);

  Serial.println();
  Serial.print("Cinnecting to");
  Serial.println(ssid);

  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  if(!client.connect(gost,port)){
    Serial.println("connection failed");
    return;
  }
}
void loop(){
  int n=0;
  if(Serial.readBytes(wbuf, 20)){
  wbuf[n] = '\0';
  Serial.print(wbuf);
  client.print(wbuf);
  Serial.println("send ok");

  }
  delay(1000);
}