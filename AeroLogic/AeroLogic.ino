// AeroLogic Garden Controller with telnet UI - Aeroponics μC for home gardens
//    Copyright (C) 2020  Nicholas Aliabadi
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Affero General Public License as published
//    by the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#include <WiFi.h>
#include <cstring>
#include "AeroGlobals.h"
#include "AeroConfig.h"
//#include "AeroTime.h"
//#include "AeroUI.h"
//#include "AeroWorker.h"


  
void setup() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  Serial.begin(115200);
  delay(10);
  pinMode(m1Pin, OUTPUT);
  pinMode(m2Pin, OUTPUT);
  pinMode(m3Pin, OUTPUT);
  pinMode(m4Pin, OUTPUT);
  pinMode(lightsPin, OUTPUT);
  
  //connect to WiFi
  Serial.println();
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  //start the server
  server.begin();
  server.setNoDelay(true);

  Serial.print("Ready! Use 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.println(" 23' to connect");
  
  printLocalTime();

  checkTime();
  updateTimers(gTimers);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    //check if there are any new clients
    if (server.hasClient()) {
      if (!serverClient || !serverClient.connected()) {
        if (serverClient) serverClient.stop();
        serverClient = server.available();
        Serial.print("New client: ");
        Serial.print(serverClient.remoteIP());
        delay(1);
        displayWelcome();
        serverClient.flush();
        delay(100);
        getCmd();
      }
    }
    serverClient.flush();
    getCmd();
    
  }
  else {
    Serial.println("WiFi not connected!");
    if (serverClient) serverClient.stop();
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println(" CONNECTED");
    delay(1000);
  }

  checkTime();
}
