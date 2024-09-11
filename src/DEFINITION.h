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
// Indirizzo in memoria EEPROM del QoS
#define EEPROM_MQTT_QOS_ADDRESS 16
// Indirizzo in memoria EEPROM del nome del client (100byte)
#define EEPROM_CLIENT_ADDRESS 200
// Indirizzo in memoria EEPROM del will (200 byte)
#define EEPROM_WILL_TOPIC_ADDRESS 300
#define EEPROM_WILL_MESSAGE_ADDRESS 400
// Indirizzo in memoria EEPROM del nome del topic per il publish (100byte)
#define EEPROM_TOPIC_ADDRESS 500
// Indirizzo in memoria EEPROM dello user name per l'mqtt (100byte)
#define EEPROM_USER_NAME_ADDRESS 600
// Indirizzo in memoria EEPROM della password per l'mqtt (100byte)
#define EEPROM_PASSWORD_ADDRESS 700

#endif