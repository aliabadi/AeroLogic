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
