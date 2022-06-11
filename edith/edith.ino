/* ~~~~~~~~~~ Includes ~~~~~~~~~~ */
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "display.h"

#define WIFI_POLL_ATTEMPTS 10

// Replace with your network credentials
const char* ssid = "Not Skynet";
const char* password = "arnold1984";
char txmsg[75];

int bat = 0;
bool face_detected = false;
String face_s;
bool bright_display = false;
String display_s;

void wifi_init();

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   /*.button:hover {background-color: #0f8b8d}*/
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>EDITH</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>EDITH</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Info</h2>
      <p class="state">X: <span id="data0">%DATA0%</span> </p>
      <p class="state">Y: <span id="data1">%DATA1%</span> </p>
      <p class="state">W: <span id="data2">%DATA2%</span> </p>
      <p class="state">H: <span id="data3">%DATA3%</span> </p>
      <p class="state">Battery: <span id="data4">%DATA4%</span>&#37</p>
      <p class="state">Face Detected? <span id="data5">%DATA5%</span> </p>
      <p class="state">Display On? <span id="data6">%DATA6%</span> </p>
      <p><button id="bright_button" class="button">Toggle Brightness</button></p>
      <p><button id="power_button" class="button">Toggle On/Off</button></p>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    document.getElementById('data0').innerHTML = event.data.substring(0, 3);
    document.getElementById('data1').innerHTML = event.data.substring(3, 6);
    document.getElementById('data2').innerHTML = event.data.substring(6, 9);
    document.getElementById('data3').innerHTML = event.data.substring(9, 12);
    document.getElementById('data4').innerHTML = event.data.substring(12, 15);
    document.getElementById('data5').innerHTML = event.data.substring(15, 18);
    document.getElementById('data6').innerHTML = event.data.substring(18, 21);
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('bright_button').addEventListener('click', toggle_bright);
    document.getElementById('power_button').addEventListener('click', toggle_power);
  }
  function toggle_bright(){
    websocket.send('toggle_bright');
  }
  function toggle_power(){
    websocket.send('toggle_power');
  }
</script>
</body>
</html>
)rawliteral";

void notifyClients() {
  
  if(face_detected){
    face_s = "yes";
  } else {
    face_s = "no ";
  }

  if(display_on){
    display_s = "yes";
  } else {
    display_s = "no ";
  }
  sprintf(txmsg, 
          "%03d%03d%03d%03d%03d%s%s", 
          rect.x, rect.y, rect.w, rect.h, bat, face_s, display_s);
  ws.textAll(txmsg);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle_bright") == 0) {
      //ledState = !ledState;
      //sensors[14] = 0;
      bright_display = !bright_display; 
      if(bright_display){
        set_brightness(bright);
      } else {
        set_brightness(dim);
      }
      notifyClients();
    } else if (strcmp((char*)data, "toggle_power") == 0) {
      //ledState = !ledState;
      //sensors[14] = 0;
      display_on = !display_on;
      
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    return "?????";
  }
  return String();
}

//#define SER_DEBUG
struct my_timer_t {
  unsigned long ref = 0;
  unsigned long dur = 100;
} t1, t2, t3, t4;

void setup() {

  Serial.begin(115200);
  Serial2.begin(115200);
  Serial2.setTimeout(3);
  display_setup();
  wifi_init();
  t1.dur = 25;
  t2.dur = 100;
  t3.dur = 500;
  t4.dur = 1000;
  t1.ref = millis();
  t2.ref = millis();
  t3.ref = millis();
  t4.ref = millis();

}

void loop() {
  
  if(millis() >= t1.ref + t1.dur){
    t1.ref = millis();
    display_loop();
  }

  if(millis() >= t2.ref + t2.dur){
    t2.ref = millis();

    String rxmsg;
    rxmsg = Serial2.readString();
    if(!rxmsg.equals("")){
      
      face_detected = true;

      //Serial.printf("X: %d Y: %s W: %s H: %s \r\n", (int)(rxmsg.substring(0, 3).toInt()), rxmsg.substring(4,7), rxmsg.substring(8,11), rxmsg.substring(12,15));
      rect.x = (int)(rxmsg.substring(0, 3).toInt());
      rect.x = map(rect.x, 0, 300, 50, 128); 
      rect.y = (int)(rxmsg.substring(4, 7).toInt());
      rect.y = (56 - rect.y*56/200);
      rect.w = (int)(rxmsg.substring(8, 11).toInt());
      rect.w = rect.w*128/300;
      rect.h = (int)(rxmsg.substring(12, 15).toInt());
      rect.h = rect.h*56/200;
      Serial.printf("X: %d Y: %d W: %d H: %d \r\n", rect.x, rect.y, rect.w, rect.h);
    } else {
      face_detected = false;
    }
    
  }

  if(millis() >= t3.ref + t3.dur){
    t3.ref = millis();
    ws.cleanupClients();
    notifyClients();
    
  }

  if(millis() >= t4.ref + t4.dur){
    t4.ref = millis();
    float vbat = analogRead(A13);
    //Serial.println(vbat);
    vbat = 6.6*vbat/4096.0;
    bat = (int)(vbat*100);
    bat = map(bat, 320, 380, 0, 100);
    
  }
  
}

void wifi_init(){
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED) {
    attempts++;
    delay(1000);
    Serial.println("Connecting to WiFi..");
    if(attempts >= WIFI_POLL_ATTEMPTS){
      Serial.println("Couldn't connect, restarting...");
      delay(100);
      ESP.restart();
    }
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Start server
  server.begin();
}
