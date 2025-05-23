/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-ds1307-real-time-clock-rtc-arduino/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.  
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>
#include <time.h>
#include <RTClib.h>  // For DS3231 or DS1307 RTC module

// Enter your Wi-Fi credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// NTP server and timezone details
const char* ntpServer = "pool.ntp.org";
const char* timezone = "WET0WEST,M3.5.0/1,M10.5.0";  
// Check list of timezones here: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

// RTC object (change to RTC_DS3231 for DS3231 module)
RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Track last sync time
struct tm timeinfo;
unsigned long lastSyncMillis = 0;  // Last sync time in milliseconds

void setup() {
  Serial.begin(115200);
  initWiFi();

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);  // Stop if RTC is not found
  }

  // Sync the RTC at startup
  syncTime();
}

void loop() {
  checkTimeAndSync();  // Check if 1 hour has passed and sync if necessary

  // Get current time from RTC
  DateTime now = rtc.now();

  // Format the time as strings
  String yearStr = String(now.year());
  String monthStr = (now.month() < 10 ? "0" : "") + String(now.month());
  String dayStr = (now.day() < 10 ? "0" : "") + String(now.day());
  String hourStr = (now.hour() < 10 ? "0" : "") + String(now.hour());
  String minuteStr = (now.minute() < 10 ? "0" : "") + String(now.minute());
  String secondStr = (now.second() < 10 ? "0" : "") + String(now.second());
  String dayOfWeek = daysOfTheWeek[now.dayOfTheWeek()];

  // Print the formatted time
  String formattedTime = dayOfWeek + ", " + yearStr + "-" + monthStr + "-" + dayStr + " " + hourStr + ":" + minuteStr + ":" + secondStr;
  Serial.println(formattedTime);

  delay(10000);
}

void initWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected.");
}

void syncTime() {
  // Configure time with NTP server and get time info
  configTime(0, 0, ntpServer);
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  // Configure timezone for DST using POSIX rule
  Serial.println("Configuring timezone and daylight saving time");
  setenv("TZ", timezone, 1);
  tzset();
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain adjusted time");
    return;
  }

  Serial.println("\nTime synchronized with NTP server with timezone and DST.");
  char timeStr[64];
  strftime(timeStr, sizeof(timeStr), "%A, %B %d %Y %H:%M:%S", &timeinfo);
  Serial.print("Current time: ");
  Serial.println(timeStr);

  // Update the RTC time
  rtc.adjust(DateTime(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                      timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec));

  lastSyncMillis = millis();  // Update last sync time
}

void checkTimeAndSync() {
  // Sync with NTP server every 1 hour (3600000 ms)
  if (millis() - lastSyncMillis >= 3600000) {
    Serial.println("Synchronizing time with NTP...");
    syncTime();
  }
}
