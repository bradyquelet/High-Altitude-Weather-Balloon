#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <LoRa.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>

// ---------------- Objects ----------------
Adafruit_BME280 bme;
TinyGPSPlus gps;

// GPS wiring:
// GPS TX -> Nano D3
// GPS RX -> Nano D4
SoftwareSerial gpsSerial(3, 4); // RX, TX

// ---------------- Pin setup ----------------
const int SD_CS_PIN = 10;

#define LORA_SS 8
#define LORA_RST 9
#define LORA_DIO0 2

// ---------------- Settings ----------------
bool sdAvailable = false;
bool loraAvailable = false;

unsigned long lastLogTime = 0;
const unsigned long logInterval = 2000; // 2 seconds

// ---------------- Setup ----------------
void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Wire.begin();

  Serial.println(F("HAB Payload Starting"));

  // Important for shared SPI devices
  pinMode(SD_CS_PIN, OUTPUT);
  pinMode(LORA_SS, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);
  digitalWrite(LORA_SS, HIGH);

  // ---------- BME280 ----------
  if (!bme.begin(0x76)) {
    Serial.println(F("BME not found at 0x76, trying 0x77"));

    if (!bme.begin(0x77)) {
      Serial.println(F("BME FAIL"));
      while (1);
    }
  }

  Serial.println(F("BME OK"));

  // ---------- SD Card ----------
  if (SD.begin(SD_CS_PIN)) {
    sdAvailable = true;
    Serial.println(F("SD OK"));

    if (!SD.exists("flight.csv")) {
      File file = SD.open("flight.csv", FILE_WRITE);

      if (file) {
        file.println(F("time_s,temp_C,pressure_hPa,humidity_percent,bme_alt_m,gps_valid,gps_age_ms,lat,lon,gps_alt_m,speed_kmh,sats"));
        file.close();
        Serial.println(F("CSV header written"));
      } else {
        Serial.println(F("SD file fail"));
        sdAvailable = false;
      }
    } else {
      Serial.println(F("flight.csv exists"));
    }
  } else {
    sdAvailable = false;
    Serial.println(F("No SD - serial/LoRa only"));
  }

  // ---------- LoRa ----------
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println(F("LoRa FAIL"));
    loraAvailable = false;
  } else {
    Serial.println(F("LoRa OK"));
    loraAvailable = true;

    // Optional settings; safe starting values
    LoRa.setTxPower(17);
    LoRa.setSpreadingFactor(7);
    LoRa.setSignalBandwidth(125E3);
    LoRa.setCodingRate4(5);
  }

  Serial.println(F("Payload Ready"));
}

// ---------------- Main loop ----------------
void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  if (millis() - lastLogTime >= logInterval) {
    lastLogTime = millis();
    logAndTransmitData();
  }
}

// ---------------- Main data function ----------------
void logAndTransmitData() {
  unsigned long t = millis() / 1000;

  // ---------- BME readings ----------
  float tempC = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0;
  float humidity = bme.readHumidity();
  float bmeAlt = bme.readAltitude(1013.25);

  // ---------- GPS readings ----------
  bool gpsValid = gps.location.isValid() && gps.location.age() < 30000;
  unsigned long gpsAge = gps.location.age();

  float lat = 0.0;
  float lon = 0.0;

  if (gpsValid) {
    lat = gps.location.lat();
    lon = gps.location.lng();
  }

  float gpsAlt = gps.altitude.meters();
  float speed = gps.speed.kmph();
  int sats = gps.satellites.value();

  // ---------- Serial monitor ----------
  Serial.print(F("t="));
  Serial.print(t);

  Serial.print(F(" T="));
  Serial.print(tempC);

  Serial.print(F(" P="));
  Serial.print(pressure);

  Serial.print(F(" H="));
  Serial.print(humidity);

  Serial.print(F(" Alt="));
  Serial.print(bmeAlt);

  Serial.print(F(" GPS="));
  Serial.print(gpsValid ? F("Y") : F("N"));

  Serial.print(F(" Age="));
  Serial.print(gpsAge);

  Serial.print(F(" Lat="));
  Serial.print(lat, 6);

  Serial.print(F(" Lon="));
  Serial.print(lon, 6);

  Serial.print(F(" GPSAlt="));
  Serial.print(gpsAlt);

  Serial.print(F(" Spd="));
  Serial.print(speed);

  Serial.print(F(" Sats="));
  Serial.print(sats);

  Serial.print(F(" SD="));
  Serial.print(sdAvailable ? F("ON") : F("OFF"));

  Serial.print(F(" LoRa="));
  Serial.println(loraAvailable ? F("ON") : F("OFF"));

  // ---------- Save to SD ----------
  if (sdAvailable) {
    writeToSD(t, tempC, pressure, humidity, bmeAlt, gpsValid, gpsAge, lat, lon, gpsAlt, speed, sats);
  }

  // ---------- Transmit over LoRa ----------
  if (loraAvailable) {
    transmitLoRa(t, tempC, pressure, humidity, bmeAlt, gpsValid, gpsAge, lat, lon, gpsAlt, speed, sats);
  }
}

// ---------------- SD write function ----------------
void writeToSD(
  unsigned long t,
  float tempC,
  float pressure,
  float humidity,
  float bmeAlt,
  bool gpsValid,
  unsigned long gpsAge,
  float lat,
  float lon,
  float gpsAlt,
  float speed,
  int sats
) {
  digitalWrite(LORA_SS, HIGH); // Make sure LoRa is not selected

  File file = SD.open("flight.csv", FILE_WRITE);

  if (file) {
    file.print(t);
    file.print(",");

    file.print(tempC);
    file.print(",");

    file.print(pressure);
    file.print(",");

    file.print(humidity);
    file.print(",");

    file.print(bmeAlt);
    file.print(",");

    file.print(gpsValid ? "Y" : "N");
    file.print(",");

    file.print(gpsAge);
    file.print(",");

    file.print(lat, 6);
    file.print(",");

    file.print(lon, 6);
    file.print(",");

    file.print(gpsAlt);
    file.print(",");

    file.print(speed);
    file.print(",");

    file.println(sats);

    file.close();
  } else {
    Serial.println(F("SD write fail"));
    sdAvailable = false;
  }
}

// ---------------- LoRa transmit function ----------------
void transmitLoRa(
  unsigned long t,
  float tempC,
  float pressure,
  float humidity,
  float bmeAlt,
  bool gpsValid,
  unsigned long gpsAge,
  float lat,
  float lon,
  float gpsAlt,
  float speed,
  int sats
) {
  digitalWrite(SD_CS_PIN, HIGH); // Make sure SD is not selected

  LoRa.beginPacket();

  LoRa.print(t);
  LoRa.print(",");

  LoRa.print(tempC);
  LoRa.print(",");

  LoRa.print(pressure);
  LoRa.print(",");

  LoRa.print(humidity);
  LoRa.print(",");

  LoRa.print(bmeAlt);
  LoRa.print(",");

  LoRa.print(gpsValid ? "Y" : "N");
  LoRa.print(",");

  LoRa.print(gpsAge);
  LoRa.print(",");

  LoRa.print(lat, 6);
  LoRa.print(",");

  LoRa.print(lon, 6);
  LoRa.print(",");

  LoRa.print(gpsAlt);
  LoRa.print(",");

  LoRa.print(speed);
  LoRa.print(",");

  LoRa.print(sats);

  LoRa.endPacket();
}