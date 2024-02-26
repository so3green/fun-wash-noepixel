#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#define NUMPIXELS 144
#define PIN 5
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB+NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pixels.begin();
  pixels.setBrightness(50);
  pinMode(2,INPUT);
}

void loop() {
  pixels.clear();
  int distance = digitalRead(2);
  delay(20);
  if(distance == 1){
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
        prev = curr;
        if(when==0){
          red = 100;
          blue = 0;
          green = 100;
          for(int i=0;i<led_num;i++){
            pixels.setPixelColor(i,pixels.Color(green,red,blue));
          }
        }
        else if(when==11){
          red = 0;
          blue = 255;
          green = 0;
          for(int i=0;i<led_num;i++){
            pixels.setPixelColor(i,pixels.Color(green,red,blue));
          }
        }
        else if(when==17){
          red = 0;
          blue = 255;
          green = 105;
          for(int i=0;i<led_num;i++){
            pixels.setPixelColor(i,pixels.Color(green,red,blue));
          }
        }
        else if(when==21){
          red = 150;
          blue = 255;
          green = 255;
          for(int i=0;i<led_num;i++){
            pixels.setPixelColor(i,pixels.Color(green,red,blue));
          }
        }
        else if(when<8 || when>10){
          for(int i=0;i<6;i++){
            pixels.setPixelColor(led_num,pixels.Color(green,red,blue));
            led_num++;
          }
        }
        pixels.show();
        when++;
      }
      if(curr - times>=28000){
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
    delay(1000);
  }
  // put your main code here, to run repeatedly:
}
