#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>

Adafruit_BME280 bme;
TinyGPSPlus gps;
SoftwareSerial gpsSerial(3, 4); // RX, TX

const int SD_CS_PIN = 10;

bool sdAvailable = false;

unsigned long lastLogTime = 0;
const unsigned long logInterval = 2000;

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(9600);
  Wire.begin();

  Serial.println(F("Payload starting"));

  // Start BME280
  if (!bme.begin(0x76)) {
    if (!bme.begin(0x77)) {
      Serial.println(F("BME fail"));
      while (1); // BME is required
    }
  }

  Serial.println(F("BME OK"));

  // Try SD card, but do not stop if missing
  if (SD.begin(SD_CS_PIN)) {
    sdAvailable = true;
    Serial.println(F("SD OK"));

    File file = SD.open("flight.csv", FILE_WRITE);
    if (file) {
      file.println(F("time_s,temp_C,pressure_hPa,humidity,bme_alt_m,gps_valid,lat,lon,gps_alt_m,speed_kmh,sats"));
      file.close();
    } else {
      Serial.println(F("SD file fail"));
      sdAvailable = false;
    }
  } else {
    sdAvailable = false;
    Serial.println(F("No SD card - serial only mode"));
  }

  Serial.println(F("Ready"));
}

void loop() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  if (millis() - lastLogTime >= logInterval) {
    lastLogTime = millis();
    logData();
  }
}

void logData() {
  unsigned long t = millis() / 1000;

  float tempC = bme.readTemperature();
  float pressure = bme.readPressure() / 100.0;
  float humidity = bme.readHumidity();
  float bmeAlt = bme.readAltitude(1013.25);

  bool gpsValid = gps.location.isValid() && gps.location.age() < 5000;

  float lat = 0.0;
  float lon = 0.0;

  if (gpsValid) {
    lat = gps.location.lat();
    lon = gps.location.lng();
  }

  float gpsAlt = gps.altitude.meters();
  float speed = gps.speed.kmph();
  int sats = gps.satellites.value();

  // Serial output
  Serial.println(F("----------------------"));

  Serial.print(F("t: "));
  Serial.println(t);

  Serial.print(F("Temp C: "));
  Serial.println(tempC);

  Serial.print(F("Pressure hPa: "));
  Serial.println(pressure);

  Serial.print(F("Humidity %: "));
  Serial.println(humidity);

  Serial.print(F("BME Alt m: "));
  Serial.println(bmeAlt);

  Serial.print(F("GPS valid: "));
  Serial.println(gpsValid ? F("YES") : F("NO"));

  Serial.print(F("Lat: "));
  Serial.println(lat, 6);

  Serial.print(F("Lon: "));
  Serial.println(lon, 6);

  Serial.print(F("GPS Alt m: "));
  Serial.println(gpsAlt);

  Serial.print(F("Speed km/h: "));
  Serial.println(speed);

  Serial.print(F("Sats: "));
  Serial.println(sats);

  Serial.print(F("SD logging: "));
  Serial.println(sdAvailable ? F("ON") : F("OFF"));

  // SD logging only if card is available
  if (sdAvailable) {
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
}