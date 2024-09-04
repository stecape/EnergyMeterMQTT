#include <Arduino.h>
#include <Ethernet.h>
#include "MQTT.h"
#include "WebPage.h"
#include <DueFlashStorage.h>
///////Definizioni
// Indirizzo in memoria EEPROM dell'IP (4byte)
#define EEPROM_IP_ADDRESS 0
// Indirizzo in memoria EEPROM dell'IP del server MQTT (4byte)
#define EEPROM_MQTT_IP_ADDRESS 4
// Indirizzo in memoria EEPROM dell'IP del server MQTT (2byte)
#define EEPROM_MQTT_PORT_ADDRESS 8
// Indirizzo in memoria EEPROM dell'intervallo di refresh del sensore (4byte)
#define EEPROM_INTERVAL_ADDRESS 12
// Indirizzo in memoria EEPROM delnome del client (100byte)
#define EEPROM_CLIENT_ADDRESS 200
// Indirizzo in memoria EEPROM delnome del topic per il publish (100byte)
#define EEPROM_TOPIC_ADDRESS 300

///////Inizializzazioni
// Actual timestamp
unsigned long actualTimeStamp = 0;
// Scrittura ritentiva nella flash
DueFlashStorage dueFlashStorage;

void setup() {
  Serial.begin(9600);
  Serial.println();

  Web::setup(dueFlashStorage, EEPROM_IP_ADDRESS);
  MQTT::setup(dueFlashStorage, EEPROM_MQTT_IP_ADDRESS, EEPROM_MQTT_PORT_ADDRESS, EEPROM_INTERVAL_ADDRESS, EEPROM_CLIENT_ADDRESS);

  delay(1500);
}

void loop() {
  actualTimeStamp = millis();

  Web::loopManagement(dueFlashStorage, EEPROM_IP_ADDRESS, EEPROM_MQTT_IP_ADDRESS, EEPROM_MQTT_PORT_ADDRESS, EEPROM_INTERVAL_ADDRESS);
  MQTT::loopManagement(actualTimeStamp, EEPROM_INTERVAL_ADDRESS);
}