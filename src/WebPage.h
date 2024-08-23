#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <SD.h>
#include <Ethernet.h>

void handleClient(EthernetClient client);
void saveVariable(int address, const char* value);
String readVariable(int address);
void updateVariable(int address, const char* value);

#endif