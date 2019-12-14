#include <ESP8266WiFi.h> // Include the Wi-Fi library
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

WiFiClient client;
const char* hostGet = "192.168.2.4"; 
ESP8266WebServer server; 
const char* ssid     = "DIGISOL";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "shreeram1998";     // The password of the Wi-Fi network
uint8_t pin_led = 16;
char sessionPage[] PROGMEM = R"=====(
<html>
<head>
<script>
function changeState(){
  var xhr = new XMLHttpRequest();
  var url = "/session";
  xhr.onreadystatechange = function(){
    if(this.readyState == 4 && this.status == 200){
      document.getElementById("state").innerText = this.responseText;
      }
    };
    xhr.open("GET",url,true);
    xhr.send();
  }
  function sendIP(){
  var xhr = new XMLHttpRequest();
  var url = "/getIP";
  xhr.onreadystatechange = function(){
    if(this.readyState == 4 && this.status == 200){
      document.getElementById("ip").innerText = this.responseText;
      }
    };
    xhr.open("GET",url,true);
    xhr.send();
  }
  
</script>
</head>
  <body>
    <center>
    <h1>Start or end the session</h1><br>
  
      <button onclick="changeState()" id="state"> start </button>
       <button onclick="sendIP()" id="ip"> Send my ip</button>
    
  </body>
<html>
)=====";  

//===============================================POST---DATA---==========================================================================
void postData(String ip) {

   WiFiClient clientGet;
   const int httpGetPort = 8080;

   //the path and file to send the data to:
   String urlGet = "/teacher";

 
  // We now create and add parameters:
  ip;
   
  urlGet += "/" + ip;
  Serial.println("urlget:"+urlGet);
   
      Serial.print(">>> Connecting to host: ");
      Serial.println(hostGet);
      
       if (!clientGet.connect(hostGet, httpGetPort)) {
        Serial.print("Connection failed: ");
        Serial.print(hostGet);
      } else {
          clientGet.println("GET " + urlGet + " HTTP/1.1");
          clientGet.print("Host: ");
          clientGet.println(hostGet);
          clientGet.println("User-Agent: ESP8266/1.0");
        clientGet.println("Connection: close\r\n\r\n");
          
          unsigned long timeoutP = millis();
          while (clientGet.available() == 0) {
            
            if (millis() - timeoutP > 10000) {
              Serial.print(">>> Client Timeout: ");
              Serial.println(hostGet);
              clientGet.stop();
              return;
            }
          }

          //just checks the 1st line of the server response. Could be expanded if needed.
          while(clientGet.available()){
            String retLine = clientGet.readStringUntil('\r');
            Serial.println(retLine);
            break; 
          }

      } //end client connection if else
                        
      Serial.print(">>> Closing host: ");
      Serial.println(hostGet);
          
      clientGet.stop();

}

void checkSession() {
  
  digitalWrite(pin_led,!digitalRead(pin_led));    
  String state = digitalRead(pin_led) ? "On" : "Off";
  server.send(200,"text/plain",state);
}
void ip() {

 String ip =WiFi.localIP().toString().c_str();
 server.send(200,"text/plain",ip);
  postData(ip);
}
void setup() {
  Serial.begin(9600);         // Start the Serial communication to send messages to the computer
  delay(10);
  Serial.println('\n');
  
  WiFi.begin(ssid, password);             // Connect to the network
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
  pinMode(pin_led, OUTPUT);
  //===========Server Config===========================

  server.on("/",[](){server.send_P(200,"text/html",sessionPage);});
  server.begin();
  server.on("/session",checkSession);
  server.on("/getIP",ip);
}

void loop() { 
  server.handleClient();
  }

void welcome(){
  server.send(200,"text/html","<b>Hello you are on nodemcu server</b>");
  }
