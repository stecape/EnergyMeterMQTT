#ifndef UTILITIES_H
#define UTILITIES_H

// Dimensione dell'indirizzo IP (4 byte)
#define IP_SIZE 4

#include <DueFlashStorage.h>
#include <Ethernet.h>

enum TYPES {
  STRINGA,
  INTERO,
  IP
};

void validateOrInitializeInt(DueFlashStorage dueFlashStorage, uint8_t EEPROMAddr, uint16_t& value, uint16_t defaultValue);
void validateOrInitializeIP(DueFlashStorage dueFlashStorage, uint8_t EEPROMAddr, uint8_t* ip, uint8_t* defaultIP);
void generateForm(TYPES t, DueFlashStorage dueFlashStorage, EthernetClient client, const String& title, const String& _varName, uint8_t EEPROMAddr, uint16_t min, uint16_t max);
void generateResponse(TYPES t, EthernetClient client, DueFlashStorage dueFlashStorage, String _varName, String request, int EEPROMAddr);

#endif