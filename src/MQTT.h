#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>
#include <Ethernet.h>

namespace MQTT{
    // Funzioni per inizializzare e gestire il client MQTT
    void setup(PubSubClient& client, uint8_t EEPROMMqttIpAddr, uint8_t EEPROMMqttPortAddr, uint8_t EEPROMIntervalAddr);
    void loopManagement(PubSubClient& client, unsigned long actualTimeStamp);
}

#endif