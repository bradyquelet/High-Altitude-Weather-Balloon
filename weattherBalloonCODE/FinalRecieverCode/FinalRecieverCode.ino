#include <SPI.h>
#include <LoRa.h>



#define LORA_SS 15    
#define LORA_RST 16  
#define LORA_DIO0 4   

unsigned long lastPacketTime = 0;
int packetCount = 0;

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("ESP8266 LoRa Ground Station Starting");

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check wiring.");
    while (1);
  }

  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);

  Serial.println("LoRa init OK");
  Serial.println("Waiting for telemetry...");
  Serial.println("time_s,temp_C,pressure_hPa,humidity_percent,bme_alt_m,gps_valid,gps_age_ms,lat,lon,gps_alt_m,speed_kmh,sats,rssi");
}

void loop() {
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    String message = "";

    while (LoRa.available()) {
      message += (char)LoRa.read();
    }

    int rssi = LoRa.packetRssi();

    lastPacketTime = millis();
    packetCount++;

    // Print clean CSV row plus RSSI at the end
    Serial.print(message);
    Serial.print(",");
    Serial.println(rssi);
  }
}