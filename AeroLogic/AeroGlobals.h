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

WiFiServer server(23);
WiFiClient serverClient;
//WiFiClient serverClients[MAX_SRV_CLIENTS];

static xTaskHandle timerHandle = 0;
const static int taskCore = 1;

const int m1Pin = 16;
const  int m2Pin = 17;
const  int m3Pin = 18;
const  int m4Pin = 19;
const  int m5Pin = 8
const  int propLightsPin = 7
const  int lightsPin = 4;

typedef struct Timers_t {
  int lightsOnHour = 17;
  int lightsOnMinute = 0;
  int lightsOffHour = 12;
  int lightsOffMinute = 0;

  bool mister1Active = false;
  bool mister2Active = false;
  bool mister3Active = true;
  bool mister4Active = true;
  bool mister5Active = true;

  int dayMisterInterval = 50;
  int dayMisterDuration = 800;
  int nightMisterInterval = 125;
  int nightMisterDuration = 800;

} GenericTimers_t;

GenericTimers_t gTimers;

char lightsOnHour_s[3] = "17";
char lightsOnMinute_s[3] = "00";
char lightsOffHour_s[3] = "12";
char lightsOffMinute_s[3] = "00";

char dayMisterInterval_s[5] = "50";
char dayMisterDuration_s[5] = "800";
char nightMisterInterval_s[5] = "125";
char nightMisterDuration_s[5] = "800";

char cmd[] = "x";
char tIn[] = "xxxx";
