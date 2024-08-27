#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <Ethernet.h>

namespace Web {
  // Funzioni per inizializzare e gestire la parte web
  void setup(EthernetServer& ethServer, uint8_t EEPROMIpAddr);
  void loopManagement(EthernetClient client, uint8_t EEPROMIpAddr, uint8_t EEPROMMqttIpAddr, uint8_t EEPROMMqttPortAddr);
}

  // Funzioni per salvare, leggere e aggiornare variabili intere
  void saveInt(int address, int value);
  int readInt(int address);

  // Funzioni per salvare, leggere e aggiornare variabili float
  void saveFloat(int address, float value);
  float readFloat(int address);

  // Funzioni per salvare, leggere e aggiornare variabili stringa
  void saveString(int address, const char* value);
  String readString(int address);

  // Funzioni per salvare, leggere e aggiornare variabili boolean
  void saveBin(int address, bool value);
  bool readBin(int address);

#endif