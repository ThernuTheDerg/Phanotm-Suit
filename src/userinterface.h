#ifndef CONFIGURATION_H
    #include "config.h"
#endif
#ifndef TwoWire_h
    #include <Wire.h>
#endif
#ifndef Arduino_h
    #include <Arduino.h>
#endif
#ifndef SSD1306Wire_h
    #include <SSD1306Wire.h>
#endif
#ifndef __QRCODE_H_
    #include <qrcode.h>
#endif



SSD1306Wire display(DISPLAYADRESS, 21, 22);
String displayLog="";
bool displaymode = false;

void initDisplay(){
    display.init();
    display.flipScreenVertically();
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.setFont(ArialMT_Plain_10);
    display.clear();
    display.display();
}

void drawQR(String qrbytes, String instructions){
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    char buf[32];
    qrbytes.toCharArray(buf,  qrbytes.length());
    qrcode_initText(&qrcode, qrcodeData, 3, ECC_QUARTILE, buf);//For this AP SSID: tmp PWD: phantom
    display.clear();
    display.fillRect(0, 0, 64, 64);
    display.setColor(BLACK);
    for (uint8_t y = 0; y < qrcode.size; y++) {
        for (uint8_t x = 0; x < qrcode.size; x++) {
            if(qrcode_getModule(&qrcode, x, y)){
              int y_offset = 3;
              int x_offset = 3;
              display.setPixel(x_offset+x*2,y_offset+y*2);
              display.setPixel(x_offset+x*2,y_offset+y*2+1);
              display.setPixel(x_offset+x*2+1,y_offset+y*2);
              display.setPixel(x_offset+x*2+1,y_offset+y*2+1);
            }
        }
    }
    //Instructions for user
    display.setColor(WHITE);
    display.drawString(67,0, instructions);
    display.display();
}


void printToDisplayLog(String string){

  if(!displaymode)return; //if displaymode is disabled dont print
  //Prepare Text
  displayLog += string;
  int fpos =0;
  int lines =0;
  for(int i=0; i<displayLog.length(); i++){
    if(displayLog.charAt(i) == '\n'){
      if(lines == 0)fpos =i;
      lines++;
    }
  }
  if(lines > 5){
    displayLog = displayLog.substring(fpos+1);
  }

  //Write to display
  display.clear();
  display.setColor(WHITE);
  display.drawString(0,0,displayLog);
  display.display();
}

void showUI(){
    display.clear();
    display.display();
}