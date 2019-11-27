/*
  KbdRptParser.cpp
  Keyboard key parser from USB in.
  Created by kripthor, October 2019
*/
#include "KbdRptParser.h"

KbdRptParser::KbdRptParser(SoftwareSerial *ss) {
  _softSerial = ss;
  _i = 0;
}


//Inject without logging
void KbdRptParser::serialInject(String s) {
  _softSerial->print(s);
  _softSerial->flush();
  delay(5);
}

//Inject with logging
void KbdRptParser::serialReport(String s) {
  _softSerial->print(s);
  _softSerial->flush();
  _fsLogFile = SPIFFS.open("/keyboard-log.txt", "a");
  _fsLogFile.write(s.c_str(), s.length());
  _fsLogFile.close();
}


void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
  String hex = String(key, HEX);
  if (hex.length() == 1) hex = "0" + hex;
  hex = "D" + hex;
  serialReport(hex);
}


void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
  String hex = String(key, HEX);
  if (hex.length() == 1) hex = "0" + hex;
  hex = "U" + hex;
  serialReport(hex);
}


void KbdRptParser::sendKey(uint8_t mod, uint8_t key)
{
  OnKeyDown(mod, key);
  OnKeyUp(mod, key);
}

void KbdRptParser::OnControlKeysChanged(uint8_t before, uint8_t after) {
  MODIFIERKEYS beforeMod;
  *((uint8_t*)&beforeMod) = before;

  MODIFIERKEYS afterMod;
  *((uint8_t*)&afterMod) = after;

  if (beforeMod.bmLeftCtrl != afterMod.bmLeftCtrl) {
    if (afterMod.bmLeftCtrl == 1) serialReport("DLC");
    else serialReport("ULC");
  }
  if (beforeMod.bmLeftShift != afterMod.bmLeftShift) {
    if (afterMod.bmLeftShift == 1) serialReport("DLS");
    else serialReport("ULS");
  }
  if (beforeMod.bmLeftAlt != afterMod.bmLeftAlt) {
    if (afterMod.bmLeftAlt == 1) serialReport("DLA");
    else serialReport("ULA");
  }
  if (beforeMod.bmLeftGUI != afterMod.bmLeftGUI) {
    if (afterMod.bmLeftGUI == 1) serialReport("DLG");
    else serialReport("ULG");
  }
  if (beforeMod.bmRightCtrl != afterMod.bmRightCtrl) {
    if (afterMod.bmRightCtrl == 1) serialReport("DRC");
    else serialReport("URC");
  }
  if (beforeMod.bmRightShift != afterMod.bmRightShift) {
    if (afterMod.bmRightShift == 1) serialReport("DRS");
    else serialReport("URS");
  }
  if (beforeMod.bmRightAlt != afterMod.bmRightAlt) {
    if (afterMod.bmRightAlt == 1) serialReport("DRA");
    else serialReport("URA");
  }
  if (beforeMod.bmRightGUI != afterMod.bmRightGUI) {
    if (afterMod.bmRightGUI == 1) serialReport("DRG");
    else serialReport("URG");
  }
}
