#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>

namespace MQTT{
    // Funzioni per inizializzare e gestire il client MQTT
    void setup(uint8_t EEPROMMqttIpAddr, uint8_t EEPROMMqttPortAddr);
    void loopManagement(unsigned long actualTimeStamp, uint8_t EEPROMIntervalAddr);
}

#endif