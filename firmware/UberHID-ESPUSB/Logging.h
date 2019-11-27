/*
  Logging.h
  Log helpers.
  Needs refactoring, a bunch of static functions in a .h sucks.
  Created by kripthor, October 2019
*/


#ifndef Logging_h
#define Logging_h

#include <FS.h>

static void LogStatus(String s) {
  File logFile = SPIFFS.open("/status-log.txt", "a");  
  logFile.write(s.c_str(),s.length());
  logFile.close();
}


#endif
