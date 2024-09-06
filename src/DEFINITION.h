#ifndef DEFINITION_H
#define DEFINITION_H

#include <DueFlashStorage.h>
#include <Ethernet.h>

// Scrittura ritentiva nella flash
extern DueFlashStorage dueFlashStorage;

///////Definizioni
//Indirizzi in memoria
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

#endif