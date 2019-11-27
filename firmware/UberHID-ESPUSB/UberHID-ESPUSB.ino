
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include "Config.h"
#include "UHWebServer.h"
#include "KbdRptParser.h"
#include "Ota.h"
#include "Inject.h"


SoftwareSerial softSerial(RXPINLEO, TXPINLEO);
USB     Usb;
HIDBoot<USB_HID_PROTOCOL_KEYBOARD> HidKeyboard(&Usb);
KbdRptParser kparser(&softSerial);

void setup()
{

  Serial.begin(115200);
  softSerial.begin(9600);
  SPIFFS.begin();
  delay(500);
  
  kparser.serialReport("INI");

  delay(500);
  while (Usb.Init() == -1) {
    kparser.serialReport("ERR");
    delay(200);
  }
  delay(100);
  HidKeyboard.SetReportParser(0, &kparser);
  kparser.serialReport("OK<");
  kparsers = &kparser;

  WiFi.softAP(WIFISSID, WIFIPASS, WIFICHANNEL, WIFISSIDHIDDEN);  // Start the access point
  startOTA();                  // Start the OTA service
  startUHWebServer();          // Start the HTTP server
}

void loop()
{
  Usb.Task();
  handleClientUHWeb();
  ArduinoOTA.handle(); // listen for OTA events

  
}
