//LCD libraries
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//WIFI libraries
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

//json lib
#include <ArduinoJson.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x3F, 21, 4);

//WIFI
ESP8266WiFiMulti WiFiMulti;
// Wifi settings
const char* ssid = "cubedwolf";
const char* password =  "kaasblokje12";
const int charsinline = 21;
const int lines = 4;

//const char* api_url = "http://192.168.2.12/Arduino/api_test";
const char* api_url = "http://api.vanhoof.tech/test";

int timer = 0;
String firstRow = "";
String secondRow = "";


void setup()
{

   Serial.begin(115200);
   
   setupLCD();
   setupWifi();
	
}


void setupWifi(){
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);
  Serial.println(WiFi.macAddress());
  Serial.print("ChipId: ");
  Serial.println(ESP.getChipId());
  
}

void setupLCD(){
  // initialize the LCD
  Wire.begin(D2, D1);
  lcd.begin(0x3F, charsinline, 4);

  // Turn on the blacklight and print a message.
  lcd.backlight();
  printText("Fucking finaly... :D");
}

void loop()
{

   if(WiFi.status()== WL_CONNECTED){
    if(timer==0){
      getNewMessage();
      timer = 60;
    }else{
      scroll();
      timer = timer -1;
    }
    delay(500);
   }else{
    printText("No internet...");
    WiFiMulti.run();
    delay(5000);
   }
}


//Wifi management

void getNewMessage(){
 if(WiFi.status()== WL_CONNECTED){
   HTTPClient http;
   http.begin(api_url);
   http.addHeader("Content-Type", "application/json");

   int httpResponseCode = http.GET();
   if(httpResponseCode>0){
 
    String response = http.getString();   
 
    Serial.println(httpResponseCode);
    Serial.println(response);         
    handleJson(response); 
 
   }else{
    Serial.print("Error on sending PUT Request: ");
    Serial.println(httpResponseCode);
   }
 }
}

void handleJson(String rawJson){

  const size_t capacity = 7*JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(7) + JSON_OBJECT_SIZE(1) + 380;

  DynamicJsonDocument doc(capacity);
  DeserializationError error = deserializeJson(doc, rawJson);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return;
  }

  JsonArray messages = doc["messages"];
  const char* messages_0_0 = messages[0][0];
  const char* messages_0_1 = messages[0][1];
 
  Serial.print(messages_0_0);
  printText(messages_0_0);
  printTextRow2(messages_0_1);
  
}

//LCD management

void printText(String text){
  lcd.clear();
  lcd.home();
  lcd.print(text);
  firstRow = text + "  ";
}

void printTextRow2(String text){
  lcd.clear();
  lcd.setCursor(0,1);  
  lcd.print(text);
  secondRow = text + "  ";
  
}

void scroll(){
  if(firstRow.length()>charsinline-1){
    lcd.setCursor(0,0);
    firstRow = ScrollTxt(firstRow);
    lcd.print(firstRow.substring(0,charsinline-1));
  }

  if(secondRow.length()>charsinline-1){
    lcd.setCursor(0,0);
    secondRow = ScrollTxt(secondRow);
    lcd.print(secondRow.substring(0,charsinline-1));
  }
   
  
}

String ScrollTxt(String txt){
 return txt.substring(1,txt.length()) + txt.substring(0,1);
}
