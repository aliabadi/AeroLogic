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
#include "time.h"

static int taskCore = 1;

typedef struct Timers_t {
  int lightsOnHour = 17;
  int lightsOnMinute = 0;
  int lightsOffHour = 12;
  int lightsOffMinute = 0;

  bool mister1Active = false;
  bool mister2Active = false;
  bool mister3Active = true;
  bool mister4Active = true;
  bool isDay = false;

  int dayMisterInterval = 50;
  int dayMisterDuration = 800;
  int nightMisterInterval = 125;
  int nightMisterDuration = 800;
} GenericTimers_t;

static char lightsOnHour_s[3] = "17";
static char lightsOnMinute_s[3] = "00";
static char lightsOffHour_s[3] = "12";
static char lightsOffMinute_s[3] = "00";

static char dayMisterInterval_s[5] = "50";
static char dayMisterDuration_s[5] = "800";
static char nightMisterInterval_s[5] = "125";
static char nightMisterDuration_s[5] = "800";

static bool sunChanged = false;

static GenericTimers_t gTimers;

static char cmd[] = "x";
static char tIn[] = "xxxx";

const int m1Pin = 16;
const int m2Pin = 17;
const int m3Pin = 18;
const int m4Pin = 19;
const int lightsPin = 4;

WiFiServer server(23);
//WiFiClient serverClients[MAX_SRV_CLIENTS];
WiFiClient serverClient;

static xTaskHandle timerHandle = 0;

void setup() {
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

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
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

void checkTime(){
  int onHour = gTimers.lightsOnHour;
  int onMin = gTimers.lightsOnMinute;
  int offHour = gTimers.lightsOffHour;
  int offMin = gTimers.lightsOffMinute;

  int onTime = onHour*100 + onMin;
  int offTime = offHour*100 + offMin;

  int curTime = getTime();

  if(onTime > offTime) {
    if(curTime > onTime || curTime < offTime) { 
      if(!gTimers.isDay){
        gTimers.isDay = true;
        updateTimers(gTimers);
      }
      digitalWrite(lightsPin, HIGH);
//      Serial.print("curTime: ");
//      Serial.print(curTime);
//      Serial.print("  onTime: ");
//      Serial.print(onTime);
//      Serial.print("  offTime: ");
//      Serial.print(offTime);
//      Serial.println("  --  lights on");
      delay(1000);
    } else {
      if(gTimers.isDay){
        gTimers.isDay = false;
        updateTimers(gTimers);
      }
      digitalWrite(lightsPin, LOW);
//      Serial.print("curTime: ");
//      Serial.print(curTime);
//      Serial.print("  onTime: ");
//      Serial.print(onTime);
//      Serial.print("  offTime: ");
//      Serial.print(offTime);
//      Serial.println("   --  lights off");
      delay(1000);
    }
  } else if(onTime < offTime) {
    if(curTime > onTime && curTime < offTime) {
      if(!gTimers.isDay){
        gTimers.isDay = true;
        updateTimers(gTimers);
      }
      digitalWrite(lightsPin, HIGH);
//      Serial.print("curTime: ");
//      Serial.print(curTime);
//      Serial.print("  onTime: ");
//      Serial.print(onTime);
//      Serial.print("  offTime: ");
//      Serial.print(offTime);
//      Serial.println("  --  lights on");
      delay(1000);
    } else {
      if(gTimers.isDay){
        gTimers.isDay = false;
        updateTimers(gTimers);
      }
      digitalWrite(lightsPin, LOW);
//      Serial.print("curTime: ");
//      Serial.print(curTime);
//      Serial.print("  onTime: ");
//      Serial.print(onTime);
//      Serial.print("  offTime: ");
//      Serial.print(offTime);
//      Serial.println("   --  lights off");
      delay(1000);
    }
  }
}
//  if(curTime < offTime && curTime > onTime) {
//    gTimers.isDay = true;
//    digitalWrite(lightsPin, HIGH);
//    digitalWrite(lightsPin, LOW);
//    Serial.print("curTime: ");
//    Serial.print(curTime);
//    Serial.print("  onTime: ");
//    Serial.print(onTime);
//    Serial.print("  offTime: ");
//    Serial.print(offTime);
//    Serial.println("  --  lights on");
//    delay(1000);
//  } else {
//    gTimers.isDay = false;
//    digitalWrite(lightsPin, LOW);
//    Serial.print("curTime: ");
//    Serial.print(curTime);
//    Serial.print("  onTime: ");
//    Serial.print(onTime);
//    Serial.print("  offTime: ");
//    Serial.print(offTime);
//        Serial.println("   --  lights off");
//    delay(1000);
//  }
//}

//  if(onTime == getTime()){
//    sunChanged = true;
//    digitalWrite(lightsPin, HIGH);
//    gTimers.isDay = true;
//    delay(61000);
//  }
//  if(offTime == getTime()){
//    sunChanged = true;
//    digitalWrite(lightsPin, LOW);
//    gTimers.isDay = false;
//    delay(61000);
//  }
//  delay(100);
//}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

int getTime() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return -1;
  }
  int ltime = timeinfo.tm_hour * 100 + timeinfo.tm_min;
  return ltime;
}

void displayInfo() {
  displayActiveMisters();
  displaySprayTimerInfo();
  displayLightTimerInfo();
  displayPrompt();
}


void displayWelcome() {
  const char w[100] = "Connected to flower room!\n\n\r[Press enter to continue]\n\r\0";
  int wlen = strlen(w);
  serverClient.write(w, wlen);
}

void displayPrompt() {
  const char w[200] = "Available commands:\n\r  [a] Activate/Deactivate Spray Timers\n\r  [s] Spray Timers\n\r  [l] Light Timers\n\r  [f] Fan Timers\n\r  [i] Current Info\n\r  [q] Disconnect\n\n\r$> ";
  serverClient.write(w, strlen(w));
}

void cmdPrompt() {
  const char w[4] = "#> ";
  serverClient.write(w, 4);
}

void requestData(char req[32]) {
  char r[100] = "Updating ";
  strcat(r, req);
  strcat(r, "[return to ignore]:\n\r\0");
  int wlen = strlen(r);
  serverClient.write(r, wlen);
}

void getCmd() {
  while (serverClient.connected()) {
    int i = 0;
    char c;
    while (serverClient.available()) {
      c = serverClient.read();
      cmd[i++] = c; //serverClient.read();
      if (c == '\n') break;
    }
    //      delay(10);
    if (c == '\n') break;
  }
  switch (cmd[0])
  {
    case 'i':
      //      delay(1);
      displayInfo();
      break;
    case 'a':
      activeUpdater();
      break;
    case 's':
      sprayTimerUpdater();
      break;
    case 'l':
      lightTimerUpdater();
      break;
    case 'f':
      fanTimerUpdater();
      break;
    case 'q':
      serverClient.stop();
      break;
    default:
      //      delay(1);
      displayInfo();
      break;
  }
  cmd[0] = 'x';
}

//////////////////////////////////////////////
//////////// Light Timers
////////////

void lightTimerUpdater() {
  displayLightTimerInfo();

  getOnHour();
  getOnMinute();
  getOffHour();
  getOffMinute();
  
  displayPrompt();
}

void getOnHour() {
  requestData("lights-on hour [24h]");
  getInput();

  int inputTime = atoi(tIn);
  if (gTimers.lightsOnHour != inputTime) {
    if (inputTime > 0 && inputTime < 24) {
      strcpy(lightsOnHour_s, tIn);
      gTimers.lightsOnHour = inputTime;
    }
  }
}

void getOnMinute() {
  requestData("lights-on minute");
  getInput();

  int inputTime = atoi(tIn);
  if (gTimers.lightsOnMinute != inputTime) {
    if (inputTime > 0 && inputTime < 60) {
      strcpy(lightsOnMinute_s, tIn);
      gTimers.lightsOnMinute = inputTime;
    }
  }
}

void getOffHour() {
  requestData("lights-off hour [24h]");
  getInput();

  int inputTime = atoi(tIn);
  if (gTimers.lightsOffHour != inputTime) {
    if (inputTime > 0 && inputTime < 24) {
      strcpy(lightsOffHour_s, tIn);
      gTimers.lightsOffHour = inputTime;
    }
  }
}

void getOffMinute() {
  requestData("lights-off minute");
  getInput();

  int inputTime = atoi(tIn);
  if (gTimers.lightsOffMinute != inputTime) {
    if (inputTime > 0 && inputTime < 60) {
      strcpy(lightsOffMinute_s, tIn);
      gTimers.lightsOffMinute = inputTime;
    }
  }
}

void displayLightTimerInfo() {
  const char w[50] = "\nLight Timer Data:\n\r";
  const char w1[50] = "  On Time:   ";
  const char w2[50] = "  Off Time:  ";
  const char w3[50] = "  Total Time On:   ";
  const char col[2] = ":";
  const char lf[3] = "\n\r";
  char lightinfo[256] = "";

  strcat(lightinfo, w);
  strcat(lightinfo, w1);
  strcat(lightinfo, lightsOnHour_s);
  strcat(lightinfo, col);
  strcat(lightinfo, lightsOnMinute_s);
  strcat(lightinfo, lf);
  strcat(lightinfo, w2);
  strcat(lightinfo, lightsOffHour_s);
  strcat(lightinfo, col);
  strcat(lightinfo, lightsOffMinute_s);
  strcat(lightinfo, lf);
//  strcat(lightinfo, w3);
  strcat(lightinfo, "\r\n\0");

  serverClient.write(lightinfo, strlen(lightinfo));
}

//////////////////////////////////////////////
//////////// Activate/Deactivate Misters
////////////

void activeUpdater(){
  displayActiveMisters();

  requestData("Mister# to toggle [a] toggles ALL");

  cmdReq();
}

void displayActiveMisters() {
  const char w[50] = "\nActive Misters (O = on, X = off):\n\r";
  const char w1[50] = "  #1 (front right)   : ";
  const char w2[50] = "  #2 (front left)    : ";
  const char w3[50] = "  #3 (back right)    : ";
  const char w4[50] = "  #4 (back left)     : ";
  const char r[20] = "\n\r";
  char mistinfo[256] = "";

  char m1[] = "x";
  char m2[] = "X";
  char m3[] = "X";
  char m4[] = "X";

  if(gTimers.mister1Active) m1[0] = 'O';
    else m1[0] = 'X';
  if(gTimers.mister2Active) m2[0] = 'O';
    else m2[0] = 'X';
  if(gTimers.mister3Active) m3[0] = 'O';
    else m3[0] = 'X';
  if(gTimers.mister4Active) m4[0] = 'O';
    else m4[0] = 'X';

  strcat(mistinfo, w);
  strcat(mistinfo, w1);
  strcat(mistinfo, m1);
  strcat(mistinfo, r);
  strcat(mistinfo, w2);
  strcat(mistinfo, m2);
  strcat(mistinfo, r);
  strcat(mistinfo, w3);
  strcat(mistinfo, m3);
  strcat(mistinfo, r);
  strcat(mistinfo, w4);
  strcat(mistinfo, m4);
  strcat(mistinfo, r);
  strcat(mistinfo, "\n\0");

  serverClient.write(mistinfo, strlen(mistinfo));
}

void cmdReq(){
  cmdPrompt();
  
  while (serverClient.connected()) {
    int i = 0;
    char c;
    while (serverClient.available()) {
      c = serverClient.read();
      cmd[i++] = c; //serverClient.read();
      if (c == '\n') break;
    }
    //      delay(10);
    if (c == '\n') break;
  }

  bool hasUpdate = false;
  
  switch(cmd[0])
  {
    case '1':
      if(gTimers.mister1Active) gTimers.mister1Active = false;
        else gTimers.mister1Active = true;
      hasUpdate = true;
      break;
    case '2':
      if(gTimers.mister2Active) gTimers.mister2Active = false;
        else gTimers.mister2Active = true;
      hasUpdate = true;
      break;
    case '3':
      if(gTimers.mister3Active) gTimers.mister3Active = false;
        else gTimers.mister3Active = true;
      hasUpdate = true;
      break;
    case '4':
      if(gTimers.mister4Active) gTimers.mister4Active = false;
        else gTimers.mister4Active = true;
      hasUpdate = true;
      break;
    case 'a':
      if(gTimers.mister1Active) gTimers.mister1Active = false;
      if(gTimers.mister2Active) gTimers.mister2Active = false;
      if(gTimers.mister3Active) gTimers.mister3Active = false;
      if(gTimers.mister4Active) gTimers.mister4Active = false;
      hasUpdate = true;
      break;
    default:
      displayInfo();
      break;
  }
  if(hasUpdate) updateTimers(gTimers);
}

//////////////////////////////////////////////
//////////// Spray Timers
////////////

void sprayTimerUpdater() {
  displaySprayTimerInfo();

  getDayInterval();
  getNightInterval();
  getDayDuration();
  getNightDuration();

  displayPrompt();
}

void getDayInterval() {
  requestData("daytime mist interval");
  getInput();

  int inputTime = atoi(tIn);
  if (gTimers.dayMisterInterval != inputTime) {
    if (inputTime > 0 && inputTime < 1500) {
      strcpy(dayMisterInterval_s, tIn);
      gTimers.dayMisterInterval = inputTime;
    }
  }
}

void getNightInterval() {
  requestData("nighttime mist interval");
  getInput();

  int inputTime = atoi(tIn);
  if (gTimers.nightMisterInterval != inputTime) {
    if (inputTime > 0 && inputTime < 1500) {
      strcpy(nightMisterInterval_s, tIn);
      gTimers.nightMisterInterval = inputTime;
    }
  }
}

void getDayDuration() {
  requestData("daytime mist duration");
  getInput();

  int inputTime = atoi(tIn);
  if (gTimers.dayMisterDuration != inputTime) {
    if (inputTime > 0 && inputTime < 1500) {
      strcpy(dayMisterDuration_s, tIn);
      gTimers.dayMisterDuration = inputTime;
    }
  }
}

void getNightDuration() {
  requestData("nighttime mist duration");
  getInput();

  int inputTime = atoi(tIn);
  if (gTimers.nightMisterDuration != inputTime) {
    if (inputTime > 0 && inputTime < 1500) {
      strcpy(nightMisterDuration_s, tIn);
      gTimers.nightMisterDuration = inputTime;
    }
  }
}

void displaySprayTimerInfo() {
  const char w[50] = "\nSpray Timer Data:\n\r";
  const char w1[50] = "  Day Interval:   ";
  const char w2[50] = "  Night Interval: ";
  const char w3[50] = "  Day Duration:   ";
  const char w4[50] = "  Night Duration: ";
  const char sec[20] = "s\n\r";
  const char ms[20] = "ms\n\r";
  char sprayinfo[256] = "";

  strcat(sprayinfo, w);
  strcat(sprayinfo, w1);
  strcat(sprayinfo, dayMisterInterval_s);
  strcat(sprayinfo, sec);
  strcat(sprayinfo, w2);
  strcat(sprayinfo, nightMisterInterval_s);
  strcat(sprayinfo, sec);
  strcat(sprayinfo, w3);
  strcat(sprayinfo, dayMisterDuration_s);
  strcat(sprayinfo, ms);
  strcat(sprayinfo, w4);
  strcat(sprayinfo, nightMisterDuration_s);
  strcat(sprayinfo, ms);
  strcat(sprayinfo, "\n\0");

  serverClient.write(sprayinfo, strlen(sprayinfo));
}

void fanTimerUpdater() {
  updateTimers(gTimers);
  displayPrompt();
}

void getInput() {
  cmdPrompt();

  strcpy(tIn, "xxxx");
  int i = 0;

  while (serverClient.connected()) {
    char c;
    while (serverClient.available()) {
      c = serverClient.read();

      if (c == '\n' || c == '\r') {
        //        Serial.print("i: ");
        //        Serial.print(i);
        //        if(c=='\n') Serial.println("  newline found!");
        //        if(c=='\r') Serial.println("  cr found!");
        tIn[i] = '\0';
        break;
      }
      tIn[i++] = c;
    }
    delay(10);
    if (c == '\n') break;
  }
}

//  int lightsOnHour = 17;
//  int lightsOnMinute = 0;
//  int lightsOffHour = 12;
//  int lightsOffMinute = 0;
//
//  bool mister1Active = true;
//  bool mister2Active = true;
//  bool mister3Active = true;
//  bool mister4Active = false;
//
//  int dayMisterInterval = 50;
//  int dayMisterDuration = 800;
//  int nightMisterInterval = 125;
//  int nightMisterDuration = 800;

//  int onHour = timers->lightsOnHour;
//  int onMin = timers->lightsOnMinute;
//  int offHour = timers->lightsOffHour;
//  int offMin = timers->lightsOffMinute;
//
//  int dayInt = timers->dayMisterInterval;
//  int dayDur = timers->dayMisterDuration;
//  int nightInt = timers->nightMisterInterval;
//  int nightDur = timers->nightMisterDuration;
//
//  bool m1Act = timers->mister1Active;
//  
//  Serial.println(onHour);
//  Serial.println(onMin);
//  Serial.println(offHour);
//  Serial.println(offMin);
//
//  Serial.println(dayInt);
//  Serial.println(dayDur);
//  Serial.println(nightInt);
//  Serial.println(nightDur);
//
//  Serial.println(m1Act);
  
void timerTask(void *pvParameters) {
  Serial.println("in task");
  GenericTimers_t *timers = (GenericTimers_t*) pvParameters;

//  int onHour = timers->lightsOnHour;
//  int onMin = timers->lightsOnMinute;
//  int offHour = timers->lightsOffHour;
//  int offMin = timers->lightsOffMinute;
//
//  int onTime = onHour*100 + onMin;
//  int offTime = offHour*100 + offMin;

  int dayInt = timers->dayMisterInterval;
  int dayDur = timers->dayMisterDuration;
  int nightInt = timers->nightMisterInterval;
  int nightDur = timers->nightMisterDuration;

  bool m1Act = timers->mister1Active;
  bool m2Act = timers->mister2Active;
  bool m3Act = timers->mister3Active;
  bool m4Act = timers->mister4Active;
  bool isDay = timers->isDay;

  int fireDur;
  if(isDay) fireDur = dayDur;
    else fireDur = nightDur;

  int activeCount = m1Act + m2Act + m3Act + m4Act;
  int dOffset = (activeCount *  3) + (activeCount * fireDur/1000);

//
//  Serial.println((int)m1Act);
//  Serial.println((int)m2Act);
//  Serial.println((int)m3Act);
//  Serial.println((int)m4Act);
  
  while (true) {
    Serial.println("in task loop before delay");
    Serial.print(" Day interval seconds: ");
    Serial.println(dayInt);
    Serial.print(" Offset in secvonds: ");
    Serial.println(dOffset);
    if(isDay) delay(((dayInt-dOffset)*1000));
      else delay(((nightInt-dOffset)*1000));

    Serial.println("task loop after delay");
    if(m1Act) {
      fireM1(fireDur);
      delay(3000);
    }
    if(m2Act) {
      fireM2(fireDur);
      delay(3000);
    }
    if(m3Act) {
      Serial.println("Firing 3");
      fireM3(fireDur);
      delay(3000);
    }
    if(m4Act) {
      Serial.println("Firing 4");
      fireM4(fireDur);
      delay(3000);
    }
  }
}

void fireM1(int dur){
//  Serial.print("M1 Fire ");
  digitalWrite(m1Pin,HIGH);
  delay(dur);
  digitalWrite(m1Pin,LOW);
}

void fireM2(int dur){
//    Serial.print("M2 Fire ");
  digitalWrite(m2Pin,HIGH);
  delay(dur);
  digitalWrite(m2Pin,LOW);
}

void fireM3(int dur){
    Serial.print("M3 Fire ");
  digitalWrite(m3Pin,HIGH);
  delay(dur);
  digitalWrite(m3Pin,LOW);
}

void fireM4(int dur){
    Serial.print("M4 Fire ");
  digitalWrite(m4Pin,HIGH);
  delay(dur);
  digitalWrite(m4Pin,LOW);
}

void updateTimers(GenericTimers_t timers) {
  // delete the task if it exists
  //  Serial.println("in update");
  if (!timerHandle) {} else {
    vTaskDelete(timerHandle);
  }
  //Serial.println("in update");
  xTaskCreatePinnedToCore(
    timerTask,      // Task function
    "timerTask",    // Task name
    10000,          // Stack size
    (void*)&timers,           // Task parameters
    1,              // Task priority
    &timerHandle,    // Task handle
    taskCore);             // Core pinned to

  Serial.println("Task created or updated");
}
