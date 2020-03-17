//how many clients should be able to telnet to this ESP32
//#define MAX_SRV_CLIENTS 1

const char* ssid = "FBI Surveillance Van";
const char* password = "quantumsupremacy";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -28800;
const int   daylightOffset_sec = 3600;
