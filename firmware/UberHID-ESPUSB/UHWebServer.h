/*
  UHWebServer.h
  WebServer related stuff and UI
  Needs refactoring, a bunch of static functions in a .h sucks.
  Created by kripthor, October 2019
*/

#ifndef UHWebServer_h
#define UHWebServer_h

#include "Arduino.h"
#include <FS.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include "KbdRptParser.h"
#include "Inject.h"
#include "Logging.h"


static ESP8266WebServer _server(80);
static File _fsUploadFile;
static KbdRptParser *kparsers;



static String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

static String getContentType(String filename) { // determine the filetype of a given filename, based on the extension
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}


static bool handleFileRead(String path) { // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/")) path += "index.html";          // If a folder is requested, send the index file
  String contentType = getContentType(path);             // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) { // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                         // If there's a compressed version available
      path += ".gz";                                         // Use the compressed verion
    File file = SPIFFS.open(path, "r");                    // Open the file
    size_t sent = _server.streamFile(file, contentType);    // Send it to the client
    file.close();                                          // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);   // If the file doesn't exist, return false
  return false;
}

static void handleFileUpload(){ // upload a new file to the SPIFFS
  HTTPUpload& upload = _server.upload();
  String path;
  LogStatus("handleFileUpload: "+upload.filename+"\n");
  
  if(upload.status == UPLOAD_FILE_START){
    path = upload.filename;
    if (path.length() < 1) _server.send(500, "text/plain", "500: couldn't create file");
    if(!path.startsWith("/")) path = "/"+path;
    if(!path.endsWith(".gz")) {                          // The file server always prefers a compressed version of a file 
      String pathWithGz = path+".gz";                    // So if an uploaded file is not compressed, the existing compressed
      if(SPIFFS.exists(pathWithGz))                      // version of that file must be deleted (if it exists)
         SPIFFS.remove(pathWithGz);
    }
    Serial.print("handleFileUpload Name: "); 
    Serial.println(path);
    _fsUploadFile = SPIFFS.open(path, "w");            // Open the file for writing in SPIFFS (create if it doesn't exist)
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(_fsUploadFile)
      _fsUploadFile.write(upload.buf, upload.currentSize); // Write the received bytes to the file
  } else if(upload.status == UPLOAD_FILE_END){
      if(_fsUploadFile) {                                    // If the file was successfully created
      _fsUploadFile.close();                               // Close the file again
      Serial.print("handleFileUpload Size: ");
      Serial.println(upload.totalSize);
      _server.sendHeader("Location","/spiffs");      // Redirect the client to the success page
      _server.send(302);
    } else {
      _server.send(500, "text/plain", "500: couldn't create file");
    }
  }
}

static void handleNotFound(){ // if the requested file or page doesn't exist, return a 404 not found error
  if(!handleFileRead(_server.uri())){          // check if the file exists in the flash memory (SPIFFS), if so, send it
    _server.send(404, "text/plain", "404: File Not Found");
  }
}



static void handleFileList() {
  FSInfo fs_info;
  SPIFFS.info(fs_info);
  String output = "";
  
  output += "VERSION: 0.0.1";
  output += "\ntotalBytes: " + formatBytes(fs_info.totalBytes);
  output += "\nusedBytes bytes: " + formatBytes(fs_info.usedBytes);
  output += "\nblockSize: " + formatBytes(fs_info.blockSize);
  output += "\npageSize: " + formatBytes(fs_info.pageSize);
  output += "\nmaxOpenFiles: " + formatBytes(fs_info.maxOpenFiles);
  output += "\nmaxPathLength " + formatBytes(fs_info.maxPathLength)+"\n";
  
  String path = "/";
  Dir dir = SPIFFS.openDir(path);
  path = String();

  
  while (dir.next()) {
    File entry = dir.openFile("r");
    if (output != "[") {
      output += "\n";
    }
    bool isDir = false;
    output += "{\"type\":\"";
    output += (isDir) ? "dir" : "file";
    output += "\",\"name\":\"";
    if (entry.name()[0] == '/') {
      output += &(entry.name()[1]);
    } else {
      output += entry.name();
    }
    output += "\",\"size\":\"";
     output += formatBytes(entry.size());
    output += "\"}";
    entry.close();
  }

  _server.send(200, "text/plain", output);
}


static void handleSPIFFS() {
  handleFileList();
}


//Inject a string
static void handleInject() {
  String toInject = String(_server.arg("keystrokes"));
  LogStatus("handleInject: "+toInject+"\n");
  injectString(kparsers, toInject);
  _server.sendHeader("Location","/index.html");      // Redirect the client to the success page
  _server.send(302);
}


//Inject direct USB HID key or raw control codes (MOUSE, Reset, others todo)
static void handleInjectRaw() {
  String toInject = String(_server.arg("keystrokes"));
  LogStatus("handleInjectRaw: "+toInject+"\n");
   injectRaw(kparsers, toInject);
  _server.sendHeader("Location","/index.html");      // Redirect the client to the success page
  _server.send(302);
}


static void handleMouse() {
  handleInjectRaw();
}


static void startUHWebServer() {
  _server.on("/spiffs", handleSPIFFS);   
  _server.on("/upload",  HTTP_POST, [&]() { _server.send(200, "text/plain", ""); }, handleFileUpload);   
  _server.on("/inject",  HTTP_POST, handleInject);   
  _server.on("/injectRaw",  HTTP_POST, handleInjectRaw);   
  _server.on("/mouse",  HTTP_POST, handleMouse);   
  _server.on("/upload.html",  HTTP_GET, [&]() { _server.send(200, "text/html", "<html><head><title>HTML Uploader</title></head><body><center><h1>File Upload</h1><div><p>Upload plain or .gz compressed files.</p><form action='/upload' method='post' enctype='multipart/form-data'><input type='file' name='Choose file'><input class='button' type='submit' value='Upload' name='submit'></form></div></center></body></html>"); }, handleFileUpload);   
  _server.onNotFound(handleNotFound);          
  _server.begin();                        
}

static void handleClientUHWeb() {
  _server.handleClient(); 
}


#endif
