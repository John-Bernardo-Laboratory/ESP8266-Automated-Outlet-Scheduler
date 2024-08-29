#include <ESP8266WiFi.h> 
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <time.h>
#include <PZEM004Tv30.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266mDNS.h>

#include "handleMain.h"
#include "handleOutletEditor.h"
#include "handlePortal.h"
#include "PowerConsumption.h"

PZEM004Tv30 pzem(14, 12);  
ESP8266WebServer server(80);
Adafruit_SSD1306 display(128, 64, &Wire, -1);

uint8_t OUTLET[] = { 10, 9, 2, 16 }, outletNum;
const char* dayNames[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
const int MAX_JSON_SIZE = 512, ENERGY_LIMIT_SIZE = 1024, energyLimitAddr = 2097, energyInitAddr = 3513, EEPROM_SIZE = 4096;  // 2096 - current month, 3512 - current day
unsigned long outletSta[4], prevMillis = 0;
float VOLTAGE, CURRENT, POWER, ENERGY, FREQUENCY, POWER_FACTOR, APPARENT, REACTIVE, energyInit;
int monthNumber;
bool alter = true;

struct settings {
  char ssid[48];
  char password[48];
} user_wifi = {};

struct Data {
  char timeRanges[MAX_JSON_SIZE];
};

struct DataEnergy {
  char energyLimit[ENERGY_LIMIT_SIZE];
};

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < 4; i++) {
    pinMode(OUTLET[i], OUTPUT);
    digitalWrite(OUTLET[i], LOW);
  }

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("\nSSD1306 allocation failed"));
    while (1) {}
  }
  Serial.println("\nOLED initialized");
  display.clearDisplay();
  display.display();

  EEPROM.begin(EEPROM_SIZE);
  EEPROM.get(EEPROM_SIZE - sizeof(struct settings), user_wifi);

  WiFi.mode(WIFI_STA);
  WiFi.begin(user_wifi.ssid, user_wifi.password);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connecting");
  Serial.println("Connecting to WiFi");
  display.display();

  byte tries = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    if (tries++ > 30) {
      WiFi.mode(WIFI_AP);
      WiFi.softAP("Setup Portal");

      server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", WifiSetup);
      });

      server.on("/updateWifi", HTTP_POST, handleUpdateWifi);

      server.begin();

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Can't connect to Wifi");
      Serial.println("Can't connect to Wifi");
      display.display();

      delay(1000);

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Setup Portal\n\nIP: 192.168.4.1");
      display.display();

      while (true) {
        server.handleClient();
        terminal();
      }
    }
  }
  Serial.println("WiFi connected");
  Serial.print("IP addrress: ");
  Serial.println(WiFi.localIP());

  display.setCursor(0, 0);
  display.clearDisplay();
  display.println("Synchronizing Time");
  Serial.println("Synchronizing Time");
  display.display();

  configTime(28800, 0, "time.google.com");
  while (time(nullptr) < 1000000000UL) {
    delay(1000);
  }

  Serial.println("System Started");

  server.on("/response", HTTP_GET, []() {
    server.send(200, "text/html", "loaded");
  });

  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", htmlMain);
  });
  server.on("/Editor", HTTP_GET, []() {
    server.send(200, "text/html", htmlEditor);
  });
  server.on("/PowerConsumption", HTTP_GET, []() {
    server.send(200, "text/html", PowerCons);
  });
  server.on("/updateTimeRanges", HTTP_POST, []() {
    StaticJsonDocument<MAX_JSON_SIZE> jsonBuffer;
    String updateTimeRanges = server.arg("DataTR");

    DeserializationError error = deserializeJson(jsonBuffer, updateTimeRanges);
    if (!error) {
      saveJsonToEEPROM(updateTimeRanges, server.arg("outletNum").toInt());
      server.send(200, "text/plain", "Updated");
    }
  });
  server.on("/outletNumber", HTTP_POST, []() {
    outletNum = server.arg("outlet").toInt();
    server.send(200, "text/plain", "Selected");
  });
  server.on("/powerData", HTTP_GET, []() {
    server.send(200, "application/json", PowCons());
  });
  server.on("/getTimeRanges", HTTP_GET, []() {
    server.send(200, "application/json", readJsonFromEEPROM(outletNum));
    server.send(200, "text/plain", "Recieved");
  });
  server.on("/getEnergyLimit", HTTP_GET, []() {
    server.send(200, "application/json", readEnergyLimit());
    server.send(200, "text/plain", "Recieved");
  });
  server.on("/updateEnergyLimit", HTTP_POST, []() {
    StaticJsonDocument<ENERGY_LIMIT_SIZE> jsonBuffer;
    String updateEnergyLimit = server.arg("energyLimit");

    DeserializationError error = deserializeJson(jsonBuffer, updateEnergyLimit);
    if (!error) {
      saveEnergyLimit(updateEnergyLimit);
      server.send(200, "text/plain", "Updated");
    }
  });
  server.on("/Wifi", HTTP_GET, []() {
    server.send(200, "text/html", WifiSetup);
  });
  server.on("/updateWifi", HTTP_POST, handleUpdateWifi);

  server.begin();
  MDNS.begin("outlet");
}

void loop() {
  server.handleClient();
  MDNS.update();
  PowCons();
  for (int i = 0; i < 4; i++) {
    TimeProcessing(i);
  }

  terminal();
}

void handleUpdateWifi() {
  strncpy(user_wifi.ssid, server.arg("ssid").c_str(), sizeof(user_wifi.ssid));
  strncpy(user_wifi.password, server.arg("password").c_str(), sizeof(user_wifi.password));
  user_wifi.ssid[server.arg("ssid").length()] = user_wifi.password[server.arg("password").length()] = '\0';
  EEPROM.put(EEPROM_SIZE - sizeof(struct settings), user_wifi);
  EEPROM.commit();
  delay(1000);
  ESP.restart();
}

void TimeProcessing(int selectOutlet) {
  time_t now = time(nullptr);
  struct tm* timeinfo;
  timeinfo = localtime(&now);

  monthNumber = timeinfo->tm_mon + 1;
  int dayNumber = timeinfo->tm_mday;
  const char* dayName = dayNames[timeinfo->tm_wday];
  int hour = timeinfo->tm_hour;
  int minute = timeinfo->tm_min;
  int year = timeinfo->tm_year + 1900;
  unsigned long rawTime = hour * 60 + minute;

  char formattedTime[50];
  sprintf(formattedTime, "%s,%02d-%02d-%04d  %02d:%02d", dayName, dayNumber, monthNumber, year, hour, minute);

  if (EEPROM.read(3512) != dayNumber) {
    EEPROM.write(3512, dayNumber);
    EEPROM.put(energyInitAddr, ENERGY);
    EEPROM.commit();
    Serial.println("changed");
  }

  if (EEPROM.read(2096) != monthNumber) {
    EEPROM.write(2096, monthNumber);
    EEPROM.commit();
    saveEnergy(ENERGY, monthNumber - 2);
    pzem.resetEnergy();
  }

  EEPROM.get(energyInitAddr, energyInit);
  if (processLimit(monthNumber - 1, dayNumber, ENERGY)) {
    for (int i = 0; i < 4; i++) {
      digitalWrite(OUTLET[i], 0);
      outletSta[i] = 0;
    }

    if (millis() - prevMillis > 2000) {
      prevMillis = millis();
      if (alter) {
        alter = false;
        display.setTextSize(2);
        display.println("");
        display.println("  ALERT!");
        display.display();
      } else {
        alter = true;
        display.println("");
        display.println("    Limit Reached");
        display.println("");
        display.println(" All outlets are off");
        display.display();
      }
    }
    return;
  }

  String readTimeRanges = readJsonFromEEPROM(selectOutlet);
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, readTimeRanges);

  if (error) {
    return;
  }
  if (dayName == "Sun") {
    digitalWrite(OUTLET[selectOutlet], ProcessTimeRanges(doc["sunday"], rawTime, selectOutlet));
  } else if (dayName == "Mon") {
    digitalWrite(OUTLET[selectOutlet], ProcessTimeRanges(doc["monday"], rawTime, selectOutlet));
  } else if (dayName == "Tue") {
    digitalWrite(OUTLET[selectOutlet], ProcessTimeRanges(doc["tuesday"], rawTime, selectOutlet));
  } else if (dayName == "Wed") {
    digitalWrite(OUTLET[selectOutlet], ProcessTimeRanges(doc["wednesday"], rawTime, selectOutlet));
  } else if (dayName == "Thu") {
    digitalWrite(OUTLET[selectOutlet], ProcessTimeRanges(doc["thursday"], rawTime, selectOutlet));
  } else if (dayName == "Fri") {
    digitalWrite(OUTLET[selectOutlet], ProcessTimeRanges(doc["friday"], rawTime, selectOutlet));
  } else if (dayName == "Sat") {
    digitalWrite(OUTLET[selectOutlet], ProcessTimeRanges(doc["saturday"], rawTime, selectOutlet));
  }

  if (millis() - prevMillis > 8000) {
    prevMillis = millis();
    if (alter) {
      alter = false;
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("Time Remaining");
      display.println("");
      display.println("Outlet 1: " + ((!int(outletSta[0] / 60)) ? "00" : String(int(outletSta[0] / 60))) + ":" + ((!outletSta[0] % 60) ? "00" : String(outletSta[0] % 60)));
      display.println("Outlet 2: " + ((!int(outletSta[1] / 60)) ? "00" : String(int(outletSta[1] / 60))) + ":" + ((!outletSta[1] % 60) ? "00" : String(outletSta[1] % 60)));
      display.println("Outlet 3: " + ((!int(outletSta[2] / 60)) ? "00" : String(int(outletSta[2] / 60))) + ":" + ((!outletSta[2] % 60) ? "00" : String(outletSta[2] % 60)));
      display.println("Outlet 4: " + ((!int(outletSta[3] / 60)) ? "00" : String(int(outletSta[3] / 60))) + ":" + ((!outletSta[3] % 60) ? "00" : String(outletSta[3] % 60)));
      display.display();
    } else {
      alter = true;
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println(formattedTime);
      display.print("\nIP: ");
      display.println(WiFi.localIP());
      display.println("");
      display.println("Power: " + String(POWER) + " W");
      display.println("Energy: " + String(ENERGY) + " kWh");
      display.display();
    }
  }
}

bool ProcessTimeRanges(JsonArray timeRangesArray, unsigned long currentTime, int selectOutlet) {
  for (const auto& range : timeRangesArray) {
    String timeRange = range.as<String>();
    String sT = timeRange.substring(0, 5);
    String eT = timeRange.substring(8, 13);
    unsigned long startTime = (sT.substring(0, 2).toInt()) * 60 + (sT.substring(3).toInt());
    unsigned long endTime = (eT.substring(0, 2).toInt()) * 60 + (eT.substring(3).toInt());
    if (currentTime >= startTime && currentTime <= endTime) {
      outletSta[selectOutlet] = endTime - currentTime;
      return 1;
    }
  }
  outletSta[selectOutlet] = 0;
  return 0;
}

void saveJsonToEEPROM(String json, int indexEEPROM) {
  if (json.length() >= MAX_JSON_SIZE) {
    Serial.println(F("Error: JSON size exceeds EEPROM size"));
    return;
  }

  for (int i = MAX_JSON_SIZE * indexEEPROM; i < (MAX_JSON_SIZE * (1 + indexEEPROM)); i++) {
    EEPROM.write(i, '\0');
  }
  EEPROM.commit();

  Data data;
  json.toCharArray(data.timeRanges, MAX_JSON_SIZE);

  EEPROM.put(MAX_JSON_SIZE * indexEEPROM, data);
  EEPROM.commit();
}

String readJsonFromEEPROM(int indexEEPROM) {
  Data data;
  EEPROM.get(MAX_JSON_SIZE * indexEEPROM, data);
  return String(data.timeRanges);
}

String PowCons() {
  VOLTAGE = pzem.voltage();
  VOLTAGE = zeroIfNan(VOLTAGE);
  CURRENT = pzem.current();
  CURRENT = zeroIfNan(CURRENT);
  POWER = pzem.power();
  POWER = zeroIfNan(POWER);
  ENERGY = pzem.energy();
  ENERGY = zeroIfNan(ENERGY);
  FREQUENCY = pzem.frequency();
  FREQUENCY = zeroIfNan(FREQUENCY);
  POWER_FACTOR = pzem.pf();
  POWER_FACTOR = zeroIfNan(POWER_FACTOR);
  if (POWER_FACTOR == 0)
    APPARENT = 0;
  else
    APPARENT = POWER / POWER_FACTOR;

  if (POWER_FACTOR == 0)
    REACTIVE = 0;
  else
    REACTIVE = POWER / POWER_FACTOR * sqrt(1 - sq(POWER_FACTOR));

  String jsonEnergy;
  for (int i = 0; i < 12; i++) {
    if (i == monthNumber - 1) {
      jsonEnergy += String(ENERGY, 2);
      jsonEnergy += ", ";
      continue;
    }
    jsonEnergy += String(readEnergy(i), 2);
    if (i < 11) { jsonEnergy += ", "; }
  }

  String powerData = "{\"activePower\": " + String(POWER) + ", \"voltage\": " + String(VOLTAGE) + ", \"current\": " + String(CURRENT) + ", \"frequency\": " + String(FREQUENCY) + ", \"powerFactor\": " + String(POWER_FACTOR) + ", \"apparentPower\": " + String(APPARENT) + ", \"reactivePower\": " + String(REACTIVE) + ", \"energy\": [" + jsonEnergy + "]}";
  return powerData;
}

float zeroIfNan(float v) {
  if (isnan(v))
    v = 0;
  return v;
}

void saveEnergy(float energyVar, int monthNum) {
  monthNum = (monthNum < 0) ? 11 : monthNum;
  int addr = 2048 + (monthNum)*4;
  EEPROM.put(addr, energyVar);
  EEPROM.commit();
}

float readEnergy(int monthNum) {
  float temp = 0;
  EEPROM.get(2048 + (monthNum)*4, temp);
  return temp;
}

bool processLimit(int month, int day, float energy) {
  DynamicJsonDocument doc(ENERGY_LIMIT_SIZE);

  String energyLimit = readEnergyLimit();
  DeserializationError error = deserializeJson(doc, energyLimit);

  if (error) {
    return 0;
  }

  JsonArray sets = doc.as<JsonArray>();
  for (size_t i = 0; i < sets.size(); i++) {
    JsonArray set = sets[i].as<JsonArray>();
    if (set[0] == month && set[1] == day && (energy - energyInit > set[2])) {
      return 1;
    }
  }
  return 0;
}

String readEnergyLimit() {
  DataEnergy data;
  EEPROM.get(energyLimitAddr, data);
  return String(data.energyLimit);
}

void saveEnergyLimit(String json) {
  if (json.length() >= ENERGY_LIMIT_SIZE) {
    Serial.println(F("Error: JSON size exceeds EEPROM size"));
    return;
  }

  for (int i = energyLimitAddr; i < (energyLimitAddr + ENERGY_LIMIT_SIZE); i++) {
    EEPROM.write(i, '\0');
  }
  EEPROM.commit();

  DataEnergy data;
  json.toCharArray(data.energyLimit, ENERGY_LIMIT_SIZE);

  EEPROM.put(energyLimitAddr, data);
  EEPROM.commit();
}

void terminal() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();

    if (command == F("format all")) {
      formatTimeRanges();
      formatEnergy();
      formatEnergyLimit();
      formatCredentials();
    } else if (command == F("format credentials")) {
      formatCredentials();

    } else if (command == F("format time ranges")) {
      formatTimeRanges();

    } else if (command == F("format energy")) {
      formatEnergy();

    } else if (command == F("format energy limit")) {
      formatEnergyLimit();

    } else if (command == F("show status")) {
      for (int i = 0; i < 4; i++) {
        Serial.printf("Outlet %d: %d:%d\n", i + 1, int(outletSta[i] / 60), outletSta[i] % 60);
      }

      Serial.printf("\nVoltage: %.2f V\n", VOLTAGE);
      Serial.printf("Current: %.2f A\n", CURRENT);
      Serial.printf("Frequency: %.2f Hz\n", FREQUENCY);
      Serial.printf("Power: %.2f W\n", POWER);
      Serial.printf("Energy: %.2f kWh\n", ENERGY);
      Serial.printf("Power Factor: %.2f PF\n", POWER_FACTOR);
      Serial.printf("Apparent Power: %.2f VA\n", APPARENT);
      Serial.printf("Reactive Power: %.2f VAR\n", REACTIVE);

    } else if (command == F("show energy")) {
      const char* monthNames[] = { "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December" };
      for (int i = 0; i < 12; i++) {
        Serial.printf("%-12s: %.2f kWh\n", monthNames[i]);
      }

    } else if (command == F("show rssi")) {
      Serial.printf("RSSI: %d dBm\n", WiFi.RSSI());

    } else if (command == F("reboot")) {
      ESP.restart();
    } else if (command == F("help")) {
      Serial.println(F("format all\t\t\tRemove all user data.\n"
                       "format time ranges\t\tRemove all saved time ranges for outlets 1 to 4.\n"
                       "format credential\t\tEliminate the WiFi connection and SSID stored in the EEPROM.\n"
                       "format energy\t\t\tDelete all the recorded energy usage data.\n"
                       "format energy limit\t\tRemove all the stored energy limits.\n"
                       "show status\t\t\tDisplay all the parameters and the on/off status of the outlet.\n"
                       "show energy\t\t\tDisplay all the stored energy usage.\n"
                       "show rssi\t\t\tDisplay Received Signal Strength Indicator (RSSI) status.\n"
                       "reboot\t\t\t\tRestart the device."));
    } else {
      Serial.println(F("Unknown command"));
    }
  }
}

void formatTimeRanges() {
  for (int i = 0; i < MAX_JSON_SIZE * 4; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();

  for (int i = 0; i < 4; i++) {
    String defaultJson = F("{\"sunday\":[],\"monday\":[],\"tuesday\":[],\"wednesday\":[],\"thursday\":[],\"friday\":[],\"saturday\":[]}");
    saveJsonToEEPROM(defaultJson, i);
  }

  Serial.println(F("Time Ranges formatted successfully"));
}

void formatEnergy() {
  for (int i = 2048; i < 2096; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  Serial.println(F("Energy formatted successfully"));
}

void formatEnergyLimit() {
  for (int i = energyLimitAddr; i < energyLimitAddr + ENERGY_LIMIT_SIZE; i++) {
    EEPROM.write(i, '\0');
  }
  EEPROM.commit();
  Serial.println(F("Energy Limit formatted successfully"));
  saveEnergyLimit("[]");
}

void formatCredentials() {
  for (int i = EEPROM_SIZE - sizeof(struct settings); i <= EEPROM_SIZE; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
  Serial.println(F("Credentials formatted successfully"));
  ESP.restart();
}