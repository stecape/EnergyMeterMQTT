#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <SD.h>
#include <Ethernet.h>

// Funzioni per gestire il client
void handleClient(EthernetClient client, uint8_t EEPROMipAddr);

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