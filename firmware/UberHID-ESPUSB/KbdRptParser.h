/*
  KbdRptParser.h
  Keyboard key parser from USB in.
  Created by kripthor, October 2019
*/
#ifndef KbdRptParser_h
#define KbdRptParser_h

#include <hidboot.h>
#include <usbhub.h>
#include "Config.h"
#include <FS.h>
#include <SoftwareSerial.h>

#define KEYBUF 32



class KbdRptParser : public KeyboardReportParser
{
  public:
    KbdRptParser(SoftwareSerial *ss);
    void serialReport(String s);
    void sendKey(uint8_t mod, uint8_t key);
    void serialInject(String s);
  protected:
    SoftwareSerial *_softSerial;
    File _fsLogFile;
    byte _lastKeys[3*KEYBUF];
    byte _i;
    void OnControlKeysChanged(uint8_t before, uint8_t after);
    void OnKeyDown  (uint8_t mod, uint8_t key);
    void OnKeyUp  (uint8_t mod, uint8_t key);
};


#endif
