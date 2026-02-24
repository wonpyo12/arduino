#include <ESP8266WiFi.h>       // 변경: WiFi.h -> ESP8266WiFi.h
#include <ESP8266HTTPClient.h> // 변경: HTTPClient.h -> ESP8266HTTPClient.h
#include <WiFiClient.h>        // ESP8266에서 HTTP 통신을 위해 필요
#include <DHT.h>

#define DHTPIN D3

#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
const char WIFI_SSID[] = "5층";         
const char WIFI_PASSWORD[] = "48864886"; 

String HOST_NAME = "http://192.168.0.69:5000"; 
String PATH_NAME = "/collect?temperature=25.5&humidity=60.0"; // 주석 해제 및 설정

void setup() {
  Serial.begin(115200); // ESP8266은 보통 115200을 기본으로 많이 씁니다.
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting");
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected! IP Address: ");
  Serial.println(WiFi.localIP());
  dht.begin();
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT sensor!");
    delay(2000);
    return;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    String dynamicPath = "/collect?temperature=" + String(t) + "&humidity=" + String(h);
    // ESP8266은 http.begin에 client 객체를 같이 넣어주는 것이 안정적입니다.
    http.begin(client, HOST_NAME + dynamicPath); 
    
    int httpCode = http.GET();
    
    if(httpCode > 0) {
      if(httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
      } else {
        Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  
  delay(10000); // 10초마다 한 번씩 요청 (loop 안에서 계속 실행 방지)
}