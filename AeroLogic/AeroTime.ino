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


#include "time.h"

//    AeroLogic() {
      
//    }
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
