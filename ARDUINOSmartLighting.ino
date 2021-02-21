#include "WiFi.h"

const char* ssid     = "iPhone de Alexandra-Diana";
const char* password = "";

WiFiServer server(80);

String redString = "0";
const int redPin = 13; 
const int redChannel = 0;

String greenString = "0";
const int greenPin = 12;  
const int greenChannel = 1;

String blueString = "0";
const int bluePin = 14; 
const int blueChannel = 2;

String header;
const int freq = 5000;
const int resolution = 8;

int pos1 = 0;
int pos2 = 0;
int pos3 = 0;
int pos4 = 0;


unsigned long currentTime = millis();
unsigned long previousTime = 0; 
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
 
  ledcSetup(redChannel, freq, resolution);
  ledcSetup(greenChannel, freq, resolution);
  ledcSetup(blueChannel, freq, resolution);
  

  ledcAttachPin(redPin, redChannel);
  ledcAttachPin(greenPin, greenChannel);
  ledcAttachPin(bluePin, blueChannel);
  

  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();  

  if (client) {                            
    currentTime = millis();
    previousTime = currentTime;
    
    Serial.println("New Client.");         
    String currentLine = "";               
    
    while (client.connected() && currentTime - previousTime <= timeoutTime) {           
      currentTime = millis();
      if (client.available()) {             
        char c = client.read();            

        header += c;
        if (c == '\n') {                   
         
          if (currentLine.length() == 0) {
            
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

          
            if(header.indexOf("GET /?r") >= 0) {
              pos1 = header.indexOf('r');
              pos2 = header.indexOf('g');
              pos3 = header.indexOf('b');
              pos4 = header.indexOf('&');
              redString = header.substring(pos1+1, pos2);
              client.println(redString);
              greenString = header.substring(pos2+1, pos3);
              blueString = header.substring(pos3+1, pos4);
            
              ledcWrite(redChannel, redString.toInt());
              ledcWrite(greenChannel, greenString.toInt());
              ledcWrite(blueChannel, blueString.toInt());
            }
            break;
          } else { 
            currentLine = "";
          }
        } else if (c != '\r') {  
          currentLine += c;      
        }
      }
    }
   
    header = "";
   
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
