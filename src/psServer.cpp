#include <Arduino.h>
#include <WebServer.h>

#include "WebRadio.h"
#include "psServer.h"
#include "html.h"

extern WebServer server;
extern station_t StationList[ST_MAX];

void serverInit(void) {
  server.on("/",    handleRoot);     // home page
  server.on("/set", handleSet);      // handle inputs

  server.begin();
  Serial.println(".. HTTP Server gestartet");
}

String createInputFields() {
  String inputForm = "";
  uint16_t k = 0;

  for (int i = 0; i < ST_MAX; i++) {
    k = i + 1;
    inputForm += "<tr>";
    inputForm += "<td><label type='text' for='str" + String(k) + "'>" + String(k) + "</label></td>";
    inputForm += "<td><input type='text' id='Station_" + String(k) + "' name='St";
    inputForm +=     String(k) + "' value='[[string" + String(k) + "]]'></td>";
    inputForm += "<td><input type='text' id='url_" + String(k) + "' name='url";
    inputForm +=     String(k) + "' value='[[url" + String(k) + "]]'></td>";
    inputForm += "</tr>";
  }

  return inputForm;
}

String createWebpage(void) {
  String page = webpage;
  page.replace("%s", createInputFields());

  for (int i = 0; i < ST_MAX; i++) {  
    page.replace("[[string" + String(i + 1) +"]]", StationList[i].Name); 
    page.replace("[[url" + String(i + 1) +"]]", StationList[i].Url); 
  }

  setStationList(StationList);
  return page;
}

void handleRoot(void) {
  server.send(200, "text/html", createWebpage());
}

void handleSet(void) {
  for (int i = 0; i < ST_MAX; i++) {  
    if (server.hasArg("St" + String(i + 1))) server.arg("St" + String(i + 1)).toCharArray(StationList[i].Name, 40);  
    if (server.hasArg("url" + String(i + 1))) server.arg("url" + String(i + 1)).toCharArray(StationList[i].Url, 100);  
  }
 
  // show new data at web page
  server.send(200, "text/html", createWebpage());
}