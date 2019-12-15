#include <Adafruit_Fingerprint.h>
#include <ESP8266WiFi.h>  
#include <WebSocketsServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

WebSocketsServer webSocket = WebSocketsServer(81);

HTTPClient http;
//const char* hostGet = "myattendance98.000webhostapp.com"; 

SoftwareSerial mySerial(D4,D5);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
uint8_t command=0;

uint8_t id;
bool fu=true;
int l[10];
int ids[100];
int idcounter=0;
int counter=0;


//========================================================webServer+varibles==============================================
const char* hostGet = "192.168.2.3"; 
ESP8266WebServer server; 
const char* ssid     = "DIGISOL";         // The SSID (name) of the Wi-Fi network you want to connect to
const char* password = "shreeram1998";     // The password of the Wi-Fi network
//const char* ssid     = "realmeXT";         // The SSID (name) of the Wi-Fi network you want to connect to
//const char* password = "helloworld98";     // The password of the Wi-Fi network

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
  
  var Socket;
  function init(){
  Socket = new WebSocket('ws://'+window.location.hostname+':81/');
  Socket.onmessage =function(event){
    document.getElementById("fp").value += event.data;
    }
   }
  
</script>
</head>
  <body onload="javascript:init()">
    <center>
    <h1>Start or end the session</h1><br>
  
      <button onclick="changeState()" id="state"> start </button>
       <button onclick="sendIP()" id="ip"> Send my ip</button>
       <textarea id="fp"></textarea>
    
  </body>
<html>
)=====";  


//========================================================================================================================
//===============================================POST---DATA---==========================================================================
void postData(String ip) {

   WiFiClient clientGet;
   const int httpGetPort = 8080;

   //the path and file to send the data to:
   String urlGet = "/teacher/class";

 
  // We now create and add parameters:
   
  urlGet += "?ip=" + ip+"&classroom=C1";
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
  if(state == "Off"){
    command = 2;
    Serial.print("\n cmd: "+command);
    }else{
      command = 8;
      }
  server.send(200,"text/plain",state);
}
void ip() {

 String ip =WiFi.localIP().toString().c_str();
 server.send(200,"text/plain",ip);
  postData(ip);
}

//===============================================================================================





void setup() 
{

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
 
  postData(WiFi.localIP().toString().c_str());
  //===================================================
  finger.begin(57600);
  
  if(finger.verifyPassword()) {
    Serial.println("Fingerprint Sensor aavailable!");
  } else {
    Serial.println("No fingerprint sensor available !");
    while (1) { delay(1); }
  }
  
  finger.getTemplateCount();
  Serial.print("Templates :"); Serial.print(finger.templateCount); 
  webSocket.begin(); 
  
}

void loop()                     
{
    

  server.handleClient();
  delay(1000);
  Serial.println("command"+command);
  Serial.print("\n *************************MENU*******************");
  Serial.print("\n 1.Register \n 2.mark your attendance \n 3.Delete  \n  \n");
  
//if (client.connect("api.thingspeak.com",80)) {
//      
//      choice = 2;
//    }else{
  
    
    switch (command){
      case 1:   repeat :
             Serial.println("Registering started ...!");
             Serial.println("Enter ID for your fingerprint ...");
             id = readnumber();
             if (id == 0) 
             {
              break;
             return;
             }
             Serial.print("Registering id =");
             Serial.println(id); 
             
             while(!getFingerprintEnroll());
             
             break; 
      case 2: //verify
            //bool fu=true;
            char c;  
            do{  
              server.handleClient();
              webSocket.loop();
                 //getFingerprint();
                int fid = getFingerprintIDez();
                Serial.println(fid);
                if(fid!=-1){
                 String sid = String(fid);
//                postData(String(fid));
                webSocket.broadcastTXT(sid);
//                  webSocket.sendTXT(1, sid);
                }
                
                c=Serial.read();
                if(c=='n')
                {
                    fu=false;
                    for(int i=0;i<=counter;i++)
                    Serial.println("as"+l[i]);
                }
            }while(command==2);
            break;
      case 3:
            Serial.println("Enter id to be deleted ...");
            id = readnumber();
            if (id == 0) {
               return;
            }
            
            deleteFingerprint(id);
            break;


      case 4: //list out
              Serial.print("List of Present Students =>");
              for(int x=0;x<=counter-1;x++){
                  Serial.print(l[x]);
                  Serial.print("|");
              }
              break;
      default:
              Serial.println("WRONG CHOICE.....!!");
              break;
           }
            delay(40);//slow down the the process
}
/*
 * This function will just read the characters 
 * from serialMonitor and will convert it to 
 * int
*/
uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}


/*ENROLL
*/


uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("registering getting started for id :"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".....");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }//success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }//success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }//converted!
  Serial.print("model for id :");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("id :"); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
    ids[idcounter]=id;
    idcounter++;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
}


/////END OF ENROLL

uint8_t getFingerprint() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("id found :"); Serial.print(finger.fingerID); 
  Serial.print(" Confidence :"); Serial.println(finger.confidence); 

  return finger.fingerID;
}
  
//verify
int getFingerprintIDez(){
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  
  if (p != FINGERPRINT_OK)  {
    Serial.println("");
    return -1;
  }else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
  return -1;
  }//matched!
/*  request_string = thingSpeakAddress;
                request_string += "key=";
                request_string += "STH30P58ZN6VV1BM";
                request_string += "&";                          //Uncomment to enable thinkspeak
                request_string += "field2";
                request_string += "=";
                request_string += (finger.fingerID);
                http.begin(request_string);
                http.GET();
                http.end();*/
  Serial.print("id found :"); Serial.print(finger.fingerID); 
  Serial.print(" Confidence :"); Serial.println(finger.confidence); 
  att_list(finger.fingerID);
  return finger.fingerID; 
}

//DELETION
uint8_t deleteFingerprint(uint8_t id) {
  uint8_t p = -1;
  
  p = finger.deleteModel(id);

  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
    return p;
  }   
}

//list
void att_list (uint8_t id){
  
  l[counter]=id;
   
   counter++;
  }
