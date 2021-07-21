//ESP32 LC709203F Web server
#include "LC709203F.h"

#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "ssid";
const char* password = "password";

LC709203F gg;  // create a gas gauge object.

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .ds-labels{
      font-size: 1.5rem;
      vertical-align:middle;
      padding-bottom: 15px;
    }
  </style>
</head>
<body>
  <h2>ESP32 LC709203F Server</h2>
  <p>
    <span class="ds-labels">Cell Voltage</span> 
    <span id="cellvoltage">%CELLVOLTAGE%</span>
    <sup class="units">V</sup>
  </p>
  <p>
    <span class="ds-labels">Cell State of Charge</span>
    <span id="stateofcharge">%STATEOFCHARGE%</span>
    <sup class="units">%</sup>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("cellvoltage").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/cellvoltage", true);
  xhttp.send();
}, 5000) ;
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("stateofcharge").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/stateofcharge", true);
  xhttp.send();
}, 5000) ;
</script>
</html>)rawliteral";

// Replaces placeholder with LC709203F values
String processor(const String& var){
  //Serial.println(var);
  if(var == "CELLVOLTAGE"){
    return String( gg.cellVoltage_mV()/1000.0 );
  }
  else if(var == "STATEOFCHARGE"){
   return String( gg.cellRemainingPercent10()/10 );
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  Serial.println();
  delay(500);
  
  if (!gg.begin()) {
    while (1) delay(10);
  }
  
  Serial.print("Version: 0x"); Serial.println(gg.getICversion(), HEX);
  
  gg.setCellCapacity(LC709203F_APA_1000MAH);

  gg.setAlarmVoltage(3.4);
  gg.setCellProfile( LC709203_NOM3p7_Charge4p2 ) ;
 
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/cellvoltage", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(gg.cellVoltage_mV()/1000.0).c_str());
  });
  server.on("/stateofcharge", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(gg.cellRemainingPercent10()/10).c_str());
  });
  // Start server
  server.begin();
}
 
void loop(){
  
}
