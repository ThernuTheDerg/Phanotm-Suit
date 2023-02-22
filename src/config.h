#ifndef CONFIGURATION_H
//Configure here
    #define CONFIGURATION_H
    #define DISPLAYADRESS 0x3C //The Adress of the OLED display module (Normally 0x3C or 0x3D)
    #define CONTROLLERS {0x40, 0x41} //Put the adresses of your PWM controllers here
    #define MONITOR_SPEED 115200 //Serial monitor speed
    #define DEBUG 1 //Debug mode 0 is of 1 is on
    #define DEBUGDELAY 500 //TODO: Find some better solution


//Don't change this
#ifndef DEBUGLOG
  #define DEBUGLOG
  #ifndef MONITOR_SPEED
    #define MONITOR_SPEED 115200
  #endif
  #ifndef DEBUG
    #define DEBUG 1
  #endif
  #if DEBUG == 1
    #define logStart Serial.begin(MONITOR_SPEED);
    #define logEnd Serial.end();
    #define log(x) Serial.print(x);
    #define logln(x) Serial.println(x);
    #define logWrite(x) Serial.write(x)
  #else
    #define logStart
    #define logEnd
    #define log(x)
    #define logln(x)
    #define logWrite(x)
  #endif
#endif
#endif

