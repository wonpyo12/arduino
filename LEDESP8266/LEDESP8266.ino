/*********
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-web-server-physical-button/
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*********/

// Import required libraries
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "5층";
const char* password = "48864886";

const char* PARAM_INPUT_1 = "state"; //주고 받는 데이터

const int output = D2;
const int buttonPin = D4;

// Variables will change:
int ledState = LOW;          // led기본 상태
int buttonState;             // 버튼  상태
int lastButtonState = LOW;   // 버튼을 눌렀을때 저장

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // 바뀐시간 기록하는 변수
unsigned long debounceDelay = 50;    // 대기시간

// Create AsyncWebServer object on port 80
AsyncWebServer server(80); //웹서버 동작 

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 34px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 68px}
    input:checked+.slider {background-color: #2196F3}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>ESP Web Server</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?state=1", true); }
  else { xhr.open("GET", "/update?state=0", true); }
  xhr.send();
}

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      var inputChecked;
      var outputStateM;
      if( this.responseText == 1){ 
        inputChecked = true;
        outputStateM = "On";
      }
      else { 
        inputChecked = false;
        outputStateM = "Off";
      }
      document.getElementById("output").checked = inputChecked;
      document.getElementById("outputState").innerHTML = outputStateM;
    }
  };
  xhttp.open("GET", "/state", true);
  xhttp.send();
}, 1000 ) ;
</script>
</body>
</html>
)rawliteral";

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons ="";
    String outputStateValue = outputState();//led상태를 저장하는 역할
    //led가 켜져있으면 저장되는 값
    buttons+= "<h4>Output - GPIO 2 - State <span id=\"outputState\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"output\" " + outputStateValue + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  //빈문자열 반환
  return String();
}
// LED의 물리적인 상태를 확인해서 저장

String outputState(){
  if(digitalRead(output)){
    return "checked";
  }
  else {
    return "";
  }
  return "";
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(output, OUTPUT);
  digitalWrite(output, LOW);
  pinMode(buttonPin, INPUT);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){//상황을 기다림
    // index_html이 PROGMEM(메모리 절약형)이므로 아래 방식이 가장 안전합니다.
    AsyncWebServerResponse *response = request->beginResponse_P(200, "text/html", index_html, processor);//접속이 된다면 이렇게 응답하겠다.
    request->send(response);//전송 (버튼 코드나 html코드)
  });

  // Send a GET request to <ESP_IP>/update?state=<inputMessage>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {//PARAM_INPUT_1이변수가 state에 있으면 이거를 실행
      inputMessage = request->getParam(PARAM_INPUT_1)->value();//데이터 값을 받아옴
      inputParam = PARAM_INPUT_1;
      digitalWrite(output, inputMessage.toInt());//가져온 값을 숫자로 바꿔서 실제 핀에 출력
      ledState = !ledState;//현재 보드가 저장하고 있는 led상태변수를 반대로 뒤집음
    }
    else {
      inputMessage = "No message sent";//값이 없음
      inputParam = "none";
    }
    Serial.println(inputMessage);//연결 상태 출력
    request->send(200, "text/plain", "OK");//처리완료
  });

  //led 상태를 확인해서 0,1로 보내주는 함수
  server.on("/state", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(digitalRead(output)).c_str());//
  });
  // Start server
  server.begin();//서버시작
}
  
void loop() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);//버튼 상태 reading변수에 저장

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {//현재 (버튼)상태와 이전상태가 다르다면
    // reset the debouncing timer
    lastDebounceTime = millis();//시간을 기록하고 타이머 리셋
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {//신호 변화 확인
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {//내가 눌렀을때 상태가 버튼의 현재 상태와 같지 않으면 실행
      buttonState = reading;//버튼상태 저장하는 함수에 내가 버튼을 눌렀을때의 상태를 저장하겠다.

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {//버튼의 현재 상태가 1이면
        ledState = !ledState;//led의 현재상태를 반전시켜줌
      }
    }
  }

  // set the LED:
  digitalWrite(output, ledState);//led 현재 상태 출력

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;//지금 현재 상태를 과거 버튼상태에 저장하는 변수에 저장
}