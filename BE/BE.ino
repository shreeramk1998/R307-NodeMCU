
#include <Adafruit_Fingerprint.h>
#include <ESP8266WiFi.h>  

#include <ESP8266HTTPClient.h>

//==================-end-of-headers-=====================================================
WiFiClient client;
/*
String thingSpeakAddress= "http://api.thingspeak.com/update?";
String writeAPIKey;
String tsfield1Name;
String request_string;
*/
HTTPClient http;
//const char* hostGet = "myattendance98.000webhostapp.com"; 
const char* hostGet = "192.168.2.5"; 
SoftwareSerial mySerial(D4,D5);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


uint8_t id;
bool fu=true;
int l[10];
int ids[100];
int idcounter=0;
int counter=0;


//===============================================GET--LASTLINE---==========================================================================
String getlastline(String str)
{
  String s="";
  int len=str.length();
  for (int i=len-1;i>=len-1;i--)
  {
    if (str[i]=='\n')
    {
      break;
    }
    else
    {
      s=s+str[i];
    }
  }
  String rs="";
  for (int i=s.length()-1;i>=0;i--)
  {
    rs=rs+s[i];
  }
  return rs;
}

//===============================================POST---DATA---==========================================================================
void postData(String fid) {

   WiFiClient clientGet;
   const int httpGetPort = 80;

   //the path and file to send the data to:
   String urlGet = "/www/interface.php";

 
  // We now create and add parameters:
  String id = fid;
   
  urlGet += "?id=" + id ;
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


//===============================================GET---DATA---==========================================================================
String getData(){
  WiFiClient clientGet;
   const int httpGetPort = 80;
  String retLine = "";
  String str = "";
   //the path and file to send the data to:
   String urlGet = "/www/command.txt";   
   
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
        
           retLine = clientGet.readString();
           str = getlastline(retLine);
           
             Serial.println("command.txt(str)=>"+str);
          
//          unsigned long timeoutP = millis();
//          while (clientGet.available() == 0) {
//            
//            if (millis() - timeoutP > 20000) {
//              Serial.print(">>> Client Timeout: ");
//              Serial.println(hostGet);
//              clientGet.stop();
//              return retLine;
//            }
//          }

          //just checks the 1st line of the server response. Could be expanded if needed.
//          while(clientGet.available()){
//            String retLine = clientGet.readStringUntil('\r');
//            Serial.println(retLine);
//            break; 
//          }

      } //end client connection if else
                        
      Serial.print(">>> Closing host: ");
      Serial.println(hostGet);
          
      clientGet.stop();
    
    return str;
  }



void setup() 
{

  WiFi.disconnect();
   WiFi.begin("DIGISOL","shreeram1998");
   Serial.println();
  Serial.print("Connecting");
  while ((!(WiFi.status() == WL_CONNECTED))){
    Serial.println(".");
    delay(300);
  }
    
  Serial.begin(9600);// baud rate 
  finger.begin(57600);
  
  if(finger.verifyPassword()) {
    Serial.println("Fingerprint Sensor aavailable!");
  } else {
    Serial.println("No fingerprint sensor available !");
    while (1) { delay(1); }
  }
  
  finger.getTemplateCount();
  Serial.print("Templates :"); Serial.print(finger.templateCount); 
  
  
}

void loop()                     
{
  uint8_t command=0;
  do{
     String str = getData();
     Serial.println("command(str)="+str);
     command =atoi(str.substring(0, 1).c_str ());
     delay(15000);
  }while(0>=command>4);
  Serial.println("command"+command);
  Serial.print("\n *************************MENU*******************");
  Serial.print("\n 1.Register \n 2.mark your attendance \n 3.Delete  \n  \n");
  uint8_t choice=-1;
  
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
                 //getFingerprint();
                int fid = getFingerprintIDez();
                Serial.println(fid);
                if(fid!=-1){
                postData(String(fid));
                }
                
                c=Serial.read();
                if(c=='n')
                {
                    fu=false;
                    for(int i=0;i<=counter;i++)
                    Serial.println("as"+l[i]);
                }
            }while(fu==true);
            fu=true;
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
