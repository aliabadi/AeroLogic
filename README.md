# AeroLogic Garden μC

ESP32s microcontroller code for high pressure aeroponics system [telnet control]

I created this code in 2 days to fill an immediate need.

The solution I had been using was not as easily expandible as I would have preferred so grabbed a spare ESP32s NodeMCU I had and went to work.

The main core handles the μC loop and the second core deals with lighting up the pins and timing everything.

Right now it is one giant file, but I am moving away from the μC that currently controls the propagation area lighting, pump, and sensors and will be expanding this to work on both ESP8266 μCs as well as the ESP32.

There are currently no plans to make a web interface.  I tried that on one ESP8266 linked with an AVR board and was not happy with the results.  Maybe it would work for something very simple, but telnet is even simpler.

There are some bugs with the c++ text encoding, but it does work as intended.

Mister spray times/intervals stay synced with the lights.

## TO-DO (in rough order of priority):

- [ ] Split code modules into seperate files
- [ ] Fix user io/text buffering
- [ ] Add sensors
- [ ] Add pump control
- [ ] Add second room
- [ ] Add fan control

### CURRENT PROGRAM IN ACTION:

```
Connected to flower room!

[Press enter to continue]


Active Misters (O = on, X = off):
  #1 (front right)   : X
  #2 (front left)    : X
  #3 (back right)    : O
  #4 (back left)     : O


Spray Timer Data:
  Day Interval:   50s
  Night Interval: 125s
  Day Duration:   800ms
  Night Duration: 800ms


Light Timer Data:
  On Time:   17:00
  Off Time:  12:00

Available commands:
  [a] Activate/Deactivate Spray Timers
  [s] Spray Timers
  [l] Light Timers
  [f] Fan Timers
  [i] Current Info
  [q] Disconnect

$>
```
