#ifndef TwoWire_h
    #include <Wire.h>
#endif
#ifndef Arduino_h
    #include <Arduino.h>
#endif
#ifndef SSD1306Wire_h
    #include <SSD1306Wire.h>
#endif
#ifndef WiFi_h
    #include <WiFi.h>
#endif
#ifndef __QRCODE_H_
    #include <qrcode.h>
#endif
#ifndef _PREFERENCES_H_
    #include <Preferences.h>
#endif


#ifndef CONFIGURATION_H
  #include "config.h"
#endif
#include "userinterface.h"


//Prototypes
void initDisplay();
bool startup(void);
void sendResponse();
void wifiWebServer(void);
void makeAP(void);
void drawQR(String qrbytes, String instructions);
void printToDisplayLog(String chars);
String extractSSID(String data);
String extractPWD(String data);
String translateSpecialChars(String input);

//Globals
int controllerList[] = CONTROLLERS;
int controllers[sizeof(controllerList)];
Preferences preferences;
WiFiServer server(80);
WiFiClient client;
String http;

char ssid[256];
char pwd[256];




//Functions
bool startup(){
    logStart;
    logln("Commencing integrity check.");
    //Integrity checks
    int error = 4;
    Wire.begin();
    //Integrity check on display
    Wire.beginTransmission(DISPLAYADRESS);//Display
    error = Wire.endTransmission();
    if(error == 0){
      displaymode= true; //if display found, use it
      logln("Display: OK");
      error = 4;
    }else{
    logln("Display: ERROR");
    }
   if(displaymode)initDisplay();//only if displaymode is found
  printToDisplayLog("Commencing integrity check\nof haptic clusters...\n");
  delay(DEBUGDELAY);

    //Check conectivity of controllers
    for(int i = 0; i < sizeof(controllerList); i++){
      Wire.beginTransmission(controllerList[0]);//Display
      error = Wire.endTransmission();
      if(error == 0){
        controllerList[i] = 1;
        log("Controller" + i);
        logln(" connected.");
        printToDisplayLog("Controller" + i);
        printToDisplayLog(" connected\n");
      }else{
        controllerList[i] = 0;
        log("Controller" + i);
        logln(" not found.");
        printToDisplayLog("Controller" + i);
        printToDisplayLog(" not found.\n");
      }

      //implement motor test here

      error = 4;
      delay(DEBUGDELAY);
    }


    logln("Fetching WiFi cedentials");
    printToDisplayLog("Fetching WiFi credentials\n");
    preferences.begin("credentials", false);
    preferences.getString("ssid", "").toCharArray(ssid, 256);
    preferences.getString("pwd", "").toCharArray(pwd, 256);
    delay(DEBUGDELAY);
    if(ssid[0] == '\0'){
      printToDisplayLog("No valid credentials found\n");
      return false;
    }else{
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pwd);
    printToDisplayLog("Connecting to WiFi\n");
    printToDisplayLog("SSID: ");
    printToDisplayLog(ssid);
    printToDisplayLog("\n\n");
    for(int i = 0; i<40; i++){//Try 40 times ~20sec
      printToDisplayLog(".");
      delay(1500);
      if(WiFi.status() == WL_CONNECTED){break;}
    }
    printToDisplayLog("\n\n");
    return WiFi.status() == WL_CONNECTED;
    }
    
  }


void setup() {

  if(startup()){
    printToDisplayLog("Connection sucessful\nIP: ");
    printToDisplayLog(WiFi.localIP().toString());
    printToDisplayLog("\nDisabling Serial Connection");
    logEnd;
    delay(DEBUGDELAY);
    printToDisplayLog("\nEntering main loop");
    delay(DEBUGDELAY);
    showUI();
  }else{
    printToDisplayLog("Connection failed\nEntering WiFi setup\n");
    makeAP();
    wifiWebServer();
    printToDisplayLog("\n\n\n\n\nValues written...\nRestarting now");
    delay(2000);
    ESP.restart();
  }
  

  
}

void loop() {
}    
