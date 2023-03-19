#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <LoRaWanPacket.h>

#define LORA_CS_PIN           5
#define LORA_RESET_PIN        14
#define LORA_DIO0_PIN         2
#define IntervalSendData      5000 // 5 detik

unsigned long previousTimeMillis;

// Antares Auth
const char *deviceAddress = "00000000";
const char *networkSessionKey = "00000000000000000000000000000000";
const char *applicationSessionKey = "00000000000000000000000000000000";

struct LoRa_config{
  long Frequency;
  int SpreadingFactor;
  long SignalBandwidth;
  int CodingRate4;
  bool enableCrc;
  bool invertIQ;
  int SyncWord;
  int PreambleLength;
};
static LoRa_config txLoRa = {922000000, 12, 125000, 5, true, false, 0x34, 8};

void LoRa_setConfig(struct LoRa_config config){
  LoRa.setFrequency(config.Frequency);
  LoRa.setSpreadingFactor(config.SpreadingFactor);
  LoRa.setSignalBandwidth(config.SignalBandwidth);
  LoRa.setCodingRate4(config.CodingRate4);
  if (config.enableCrc)
    LoRa.enableCrc();
  else
    LoRa.disableCrc();
  if (config.invertIQ)
    LoRa.enableInvertIQ();
  else
    LoRa.disableInvertIQ();
  LoRa.setSyncWord(config.SyncWord);
  LoRa.setPreambleLength(config.PreambleLength);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Booting up...");
  LoRaWanPacket.personalize(deviceAddress, networkSessionKey, applicationSessionKey);
  LoRa.setPins(LORA_CS_PIN, LORA_RESET_PIN, LORA_DIO0_PIN);

  if (!LoRa.begin(txLoRa.Frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);
  }

  Serial.println("LoRa initialized success.");
  Serial.println();
}

void loop() {
   unsigned long currentTime = millis();

    if (currentTime - previousTimeMillis >= IntervalSendData) {
        previousTimeMillis = currentTime;

        LoRa_setConfig(txLoRa);
        LoRa.idle();

        LoRaWanPacket.clear();
        LoRaWanPacket.print("Hello Test");
        Serial.println("Sending data to Antares... ");
        if (LoRaWanPacket.encode()){
          LoRa.beginPacket();
          LoRa.write(LoRaWanPacket.buffer(), LoRaWanPacket.length());
          LoRa.endPacket();
        }
    }
}