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
    Serial.print(" Offset in seconds: ");
    Serial.println(dOffset);

    //////////////////////////////////////////
    /////////
    /////////  I think we can use vTaskDelayUntil here or vTaskDelay, if not.
    /////////    https://www.freertos.org/vtaskdelayuntil.html
    /////////
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
