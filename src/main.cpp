#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 144
#define PIN 5
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB+NEO_KHZ800);

#include <WiFi.h>
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"
#include "FS.h"
#include <iostream>



const char ssid[] = "ESP32";
const char pass[] = "12345678";       // パスワードは8文字以上
const IPAddress ip(192,168,123,45);
const IPAddress subnet(255,255,255,0);
const char* PARAM_MESSAGE = "message";

AsyncWebServer server(80);  
void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}
void appendFile(fs::FS &fs, const char * path,const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}
void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}
void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);
    File root = fs.open(dirname);
    if(!root){
      writeFile(SPIFFS, "/test.csv", "Hello ");
      return;
    }
}

unsigned long start =0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pixels.begin();
  pixels.setBrightness(15);
  pinMode(2,INPUT);
  pinMode(25,OUTPUT);
  pinMode(26,OUTPUT);
  pinMode(27,OUTPUT);
  pinMode(33,OUTPUT);
  ledcAttachPin(25,0);
  ledcAttachPin(26,1);
  ledcAttachPin(33,2);
  ledcSetup(0,20000,10);
  ledcSetup(1,20000,10);
  ledcSetup(2,20000,10);
  delay(50);
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  
  WiFi.softAP(ssid, pass);           // SSIDとパスの設定
  delay(100);                        // このdelayを入れないと失敗する場合がある
  WiFi.softAPConfig(ip, ip, subnet); // IPアドレス、ゲートウェイ、サブネットマスクの設定
  
  IPAddress myIP = WiFi.softAPIP();   // WiFi.softAPIP()でWiFi起動

  // 各種情報を表示
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("AP IP address: ");
  Serial.println(myIP);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", "<a href=\"/test.csv\">download</a>\n<a href=\"/set.csv\">reset</a>");
    });
    server.on("/test.csv", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/test.csv", "text/csv");
        delay(1) ;
    });
    server.on("/set.csv", HTTP_GET, [](AsyncWebServerRequest *request){
        writeFile(SPIFFS, "/test.csv", "Hello ");
        request->send(200, "text/html", "<a href=\"/test.csv\">download</a>\n<a href=\"/set.csv\">reset</a>");
        delay(1) ;

    });
    // Send a GET request to <IP>/get?message=<message>
    server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String message;
        if (request->hasParam(PARAM_MESSAGE)) {
            message = request->getParam(PARAM_MESSAGE)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, GET: " + message);
    });

    // Send a POST request to <IP>/post with a form field message set to <message>
    server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
        String message;
        if (request->hasParam(PARAM_MESSAGE, true)) {
            message = request->getParam(PARAM_MESSAGE, true)->value();
        } else {
            message = "No message sent";
        }
        request->send(200, "text/plain", "Hello, POST: " + message);
    });

    server.onNotFound(notFound);

    server.begin();
    listDir(SPIFFS, "/test.csv", 0);
    start = millis();
}

void loop() {
  pixels.clear();
  int distance = digitalRead(2);
  delay(20);
  ledcWrite(0,0);
  ledcWrite(1,0);
  ledcWrite(2,100);
  unsigned long date = millis();
  if (date-start>= 10000){
    start = millis();
    appendFile(SPIFFS,"/test.csv","\n");
  } 
  if(distance == 1){
    appendFile(SPIFFS,"/test.csv",",");
    digitalWrite(27,HIGH);
    delay(50);
    digitalWrite(27,LOW); 
    unsigned long times = millis();
    Serial.print("start");
    pixels.clear();
    int led_num = 0;
    int red = 0;
    int blue = 0;
    int green = 0;
    int when = 0;
    unsigned long prev = 0;
    unsigned long interval = 960;
    delay(1200);
    for(int i = 1;i<NUMPIXELS-1;i++){
      pixels.clear();
      pixels.setPixelColor(i-1,pixels.Color(255,255,255));
      pixels.setPixelColor(i,pixels.Color(255,255,255));
      pixels.setPixelColor(i+1,pixels.Color(255,255,255));
      pixels.show();
      delay(2);
    }
    Serial.print(when);
    for(int i=0;i<NUMPIXELS;i++){
      pixels.setPixelColor(i,pixels.Color(255,0,0));
    }
    pixels.show();
    Serial.print(when);
    while(1){
      delay(10);
      unsigned long curr = millis();
      if((curr-prev) >= interval){
        distance = digitalRead(2);
        delay(10);
        ledcWrite(0,0);
        ledcWrite(1,0);
        ledcWrite(2,100); 
        if(distance == 1){
          appendFile(SPIFFS,"/test.csv","1");
          ledcWrite(0,0);
          ledcWrite(1,100);
          ledcWrite(2,0);        
        }
        else if(distance == 0){
          appendFile(SPIFFS,"/test.csv","0");
          ledcWrite(0,100);
          ledcWrite(1,0);
          ledcWrite(2,0);        
        }
        prev = curr;
        if(when==0){
          distance = digitalRead(2);
          Serial.print(distance);
          red = 100;
          blue = 0;
          green = 100;
          for(int i=0;i<led_num;i++){
            pixels.setPixelColor(i,pixels.Color(green,red,blue));
          }
        }
        else if(when==10){
          distance = digitalRead(2);
          Serial.print(distance);
          red = 0;
          blue = 255;
          green = 0;
          for(int i=0;i<led_num;i++){
            pixels.setPixelColor(i,pixels.Color(green,red,blue));
          }
        }
        else if(when==16){
          distance = digitalRead(2);
          Serial.print(distance);
          red = 0;
          blue = 255;
          green = 105;
          for(int i=0;i<led_num;i++){
            pixels.setPixelColor(i,pixels.Color(green,red,blue));
          }
        }
        else if(when==20){
          distance = digitalRead(2);
          Serial.print(distance);
          red = 100;
          blue = 100;
          green = 100;
          for(int i=0;i<led_num;i++){
            pixels.setPixelColor(i,pixels.Color(green,red,blue));
          }
        }
        else if(when<8 || when>9){
          distance = digitalRead(2);
          Serial.print(distance);
          for(int i=0;i<6;i++){
            pixels.setPixelColor(led_num,pixels.Color(green,red,blue));
            led_num++;
          }
        }
        pixels.show();
        when++;
      }
      if(curr - times>=30000){
        for(int i=20;i>0;i--){
          for(int j=0;j<NUMPIXELS;j++){
            pixels.setPixelColor(j,pixels.Color(i*10,i*10,i*10));
          }
          pixels.show();
          delay(10);
        }
        for(int i=0;i<20;i++){
          for(int j=0;j<NUMPIXELS;j++){
            pixels.setPixelColor(j,pixels.Color(i*10,i*10,i*10));
          }
          pixels.show();
          delay(10);
        }
        for(int i=20;i>0;i--){
          for(int j=0;j<NUMPIXELS;j++){
            pixels.setPixelColor(j,pixels.Color(i*10,i*10,i*10));
          }
          pixels.show();
          delay(10);
        }
        for(int i=0;i<20;i++){
          for(int j=0;j<NUMPIXELS;j++){
            pixels.setPixelColor(j,pixels.Color(i*10,i*10,i*10));
          }
          pixels.show();
          delay(10);
        }
        for(int i=20;i>0;i--){
          for(int j=0;j<NUMPIXELS;j++){
            pixels.setPixelColor(j,pixels.Color(i*10,i*10,i*10));
          }
          pixels.show();
          delay(10);
        }
        pixels.clear();
        pixels.show();
        break;
      }
    }
    delay(4000);
  }
  // put your main code here, to run repeatedly:
}
