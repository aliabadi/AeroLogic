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

const static int m1Pin = 16;
const static int m2Pin = 17;
const static int m3Pin = 18;
const static int m4Pin = 19;
const static int lightsPin = 4;

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

static GenericTimers_t gTimers;

static char lightsOnHour_s[3] = "17";
static char lightsOnMinute_s[3] = "00";
static char lightsOffHour_s[3] = "12";
static char lightsOffMinute_s[3] = "00";

static char dayMisterInterval_s[5] = "50";
static char dayMisterDuration_s[5] = "800";
static char nightMisterInterval_s[5] = "125";
static char nightMisterDuration_s[5] = "800";

//static bool sunChanged = false;

static char cmd[] = "x";
static char tIn[] = "xxxx";
