#include <Arduino.h>
#include <Ethernet.h>
#include "MQTT.h"
#include "WebPage.h"

EthernetServer ethServer(80);

///////Definizioni
// Indirizzo in memoria EEPROM dell'IP (4byte)
#define EEPROM_IP_ADDRESS 0
// Indirizzo in memoria EEPROM dell'IP del server MQTT (4byte)
#define EEPROM_MQTT_IP_ADDRESS 4
// Indirizzo in memoria EEPROM dell'IP del server MQTT (2byte)
#define EEPROM_MQTT_PORT_ADDRESS 8
// Indirizzo in memoria EEPROM dell'intervallo di refresh del sensore (4byte)
#define EEPROM_INTERVAL_ADDRESS 10

///////Inizializzazioni
// Actual timestamp
unsigned long actualTimeStamp = 0;

void setup() {
  Serial.begin(9600);
  Serial.println();

  Web::setup(ethServer, EEPROM_IP_ADDRESS);
  MQTT::setup(EEPROM_MQTT_IP_ADDRESS, EEPROM_MQTT_PORT_ADDRESS);

  delay(1500);
}

void loop() {
  actualTimeStamp = millis();

  EthernetClient ethClient = ethServer.available();
  if(ethClient){
    Web::loopManagement(ethClient, EEPROM_IP_ADDRESS, EEPROM_MQTT_IP_ADDRESS, EEPROM_MQTT_PORT_ADDRESS);
  }
  MQTT::loopManagement(actualTimeStamp, EEPROM_INTERVAL_ADDRESS);
}