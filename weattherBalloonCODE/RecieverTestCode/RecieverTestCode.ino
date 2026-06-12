#include <SPI.h>
#include <LoRa.h>

#define LORA_SS 15    // D8 on NodeMCU
#define LORA_RST 16   // D0 on NodeMCU
#define LORA_DIO0 4   // D2 on NodeMCU

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println();
  Serial.println("LoRa receiver test starting...");

  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check wiring.");
    while (1);
  }

  Serial.println("LoRa init OK");
  Serial.println("Waiting for packets...");
}

void loop() {
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    Serial.print("Received packet: ");

    String message = "";

    while (LoRa.available()) {
      message += (char)LoRa.read();
    }

    Serial.print(message);

    Serial.print(" | RSSI: ");
    Serial.println(LoRa.packetRssi());
  }
}