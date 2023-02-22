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


Preferences preferences;
SSD1306Wire display(DISPLAYADRESS, 21, 22);
WiFiServer server(80);  
WiFiClient client;  
String http;
String displayLog="";
bool displaymode = false;
char ssid[256];
char pwd[256];

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

void makeAP(){
  WiFi.mode(WIFI_OFF);
  delay(2000);
  WiFi.mode(WIFI_AP); //Access Point mode
  WiFi.softAP("AP","Phantom1");
   IPAddress ip = WiFi.softAPIP();
   String address ="http://";
   address += ip.toString();
   address += "/";

  logln("AP open");
  drawQR("WIFI:T:WPA;S:AP;P:Phantom1;H:;;", "Scan QR to\nconnect to\nthe Vest.");
  while(WiFi.softAPgetStationNum()<1){
  vTaskDelay(50);
  }

  drawQR(address,"Scan again\nto open\nsetup menu.");
  logln(address);
}

void wifiWebServer(){
  IPAddress ip = WiFi.softAPIP();
  String s_ssid;
  String s_pwd;


  logln(ip);
  server.begin();
  for(;;){
    if(client = server.available()){
      String clientData = "";  
      while ( client.connected() ) {    // Wait until the client finish sending HTTP request  
        if ( client.available() ) {     // If there is a data,  
        char c = client.read();      //  read one character  
        http += c;            //  then parse it  
        logWrite(c);
        if (c == '\n') {           // If the character is carriage return,   
                                  //  it means end of http request from client 
          if (clientData.length() == 0) { //  Now that the clientData is cleared,  
          sendResponse();        //    perform the necessary action  
          client.println("<!DOCTYPE html>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>Phantom WiFi setup</title>");
          client.println("</head>");
          client.println("<body>");
          client.println("<h1>WiFi setup</h1>");
          client.println("<text>Enter WiFi credentials of the network you want to connect to</text><br><br>");
          client.println("<form action='/' method='POST'>");
          client.println("<input type='text' id='ssid' name='ssid' placeholder='SSID'><br><br>");
          client.println("<input type='text' id='pwd' name='pwd' placeholder='Password'><br><br>");
          client.println("<input type='submit' id='btn1' value='Connect'>");
          client.println("</form> ");
          client.println("</body>");
          client.println("</html>");
          client.println();
          break;
          } else {  
          clientData = "";       //  First, clear the clientData  
          }  
        } else if (c != '\r') {      // Or if the character is NOT new line  
          clientData += c;        //  store the character to the clientData variable  
        }  
        }
      }
      http ="";
      String data;
      for(;;){
        char x = client.read();
        if(((int)x) < 0 || ((int)x) > 254) break;
        logln(x);
        data+=x;

      }
      
      logln("<\n\n\n");
      client.stop();
      logln("Cliend disconnected");
      s_ssid = translateSpecialChars(extractSSID(data));
      s_pwd = translateSpecialChars(extractPWD(data));
      if(s_ssid.length() > 0&&s_pwd.length() > 0)break;
    }
  }
  preferences.putString("ssid", s_ssid); 
  preferences.putString("pwd", s_pwd);
  logln("SETUP SUCCESSFUL");
}

String extractSSID(String data){
  if(data.indexOf("ssid=") >= 0){
    int i = data.indexOf("ssid=");
    while(data.charAt(i) != '&' && ((int)(data.charAt(i))) > -1 && ((int)(data.charAt(i))) < 255 && i < 512 )i++;//max length 512 (Who has such a long ssid or pwd anyways? ^^)
    return data.substring(data.indexOf("ssid=")+5,i);
  }else{
  return "";
  }
}

String extractPWD(String data){
  if(data.indexOf("pwd=") >= 0){
    int i = data.indexOf("pwd=");
    while(data.charAt(i) != '&' && ((int)(data.charAt(i))) > -1 && ((int)(data.charAt(i))) < 255 && i < 512 )i++;//max length 512 (Who has such a long ssid or pwd anyways? ^^)
    return data.substring(data.indexOf("pwd=")+4,i);
  }else{
  return "";
  }
}

String translateSpecialChars(String input){
  String output;
  for(int i = 0; i<input.length(); i++){
    if(input.charAt(i) == '+'){output += ' '; i++;}
    if(input.charAt(i)=='%'){
      char translated = '-';
      char tmp[2] = {input.charAt(i+1),input.charAt(i+2)};
      if(tmp[0] == '2' && tmp[1] == '4') translated = '$';
      if(tmp[0] == '2' && tmp[1] == '6') translated = '&';
      if(tmp[0] == '2' && tmp[1] == 'B') translated = '+';
      if(tmp[0] == '2' && tmp[1] == 'C') translated = ',';
      if(tmp[0] == '2' && tmp[1] == 'F') translated = '/';
      if(tmp[0] == '3' && tmp[1] == 'A') translated = ':';
      if(tmp[0] == '3' && tmp[1] == 'B') translated = ';';
      if(tmp[0] == '3' && tmp[1] == 'D') translated = '=';
      if(tmp[0] == '3' && tmp[1] == 'F') translated = '?';
      if(tmp[0] == '4' && tmp[1] == '0') translated = '@';
      if(tmp[0] == '2' && tmp[1] == '0') translated = ' ';
      if(tmp[0] == '2' && tmp[1] == '2') translated = '"';
      if(tmp[0] == '3' && tmp[1] == 'C') translated = '<';
      if(tmp[0] == '3' && tmp[1] == 'E') translated = '>';
      if(tmp[0] == '2' && tmp[1] == '3') translated = '#';
      if(tmp[0] == '2' && tmp[1] == '5') translated = '%';
      if(tmp[0] == '7' && tmp[1] == 'B') translated = '{';
      if(tmp[0] == '7' && tmp[1] == 'D') translated = '}';
      if(tmp[0] == '7' && tmp[1] == 'C') translated = '|';
      if(tmp[0] == '5' && tmp[1] == 'C') translated = '\\';
      if(tmp[0] == '5' && tmp[1] == 'E') translated = '^';
      if(tmp[0] == '7' && tmp[1] == 'E') translated = '~';
      if(tmp[0] == '5' && tmp[1] == 'B') translated = '[';
      if(tmp[0] == '5' && tmp[1] == 'D') translated = ']';
      if(tmp[0] == '6' && tmp[1] == '0') translated = '`';
      if(tmp[0] == '2' && tmp[1] == '8') translated = '(';
      if(tmp[0] == '2' && tmp[1] == '9') translated = ')';
      //if(tmp[0] == '8' && tmp[1] == '0') translated = '€';
      if(tmp[0] == '2' && tmp[1] == '7') translated = '\'';
      if(tmp[0] == '2' && tmp[1] == '1') translated = '!';

      output += translated;
      i += 2;
    }else{
      output += input.charAt(i);
    }
  }
  return output;
}

void sendResponse(){
  client.println("HTTP/1.1 200 OK\nContent-type:text/html\nConnection: close\n");
}

