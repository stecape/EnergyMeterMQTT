#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Ethernet.h>
#include <DueFlashStorage.h>

namespace Web {
  // Funzioni per inizializzare e gestire la parte web
  void setup(DueFlashStorage dueFlashStorage, uint8_t EEPROMIpAddr);
  void loopManagement(DueFlashStorage dueFlashStorage, uint8_t EEPROMIpAddr, uint8_t EEPROMMqttIpAddr, uint8_t EEPROMMqttPortAddr, uint8_t EEPROMIntervalAddr);
}

#endif