#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>
#include <DueFlashStorage.h>

namespace MQTT{
    // Funzioni per inizializzare e gestire il client MQTT
    void setup(DueFlashStorage dueFlashStorage, uint8_t EEPROMMqttIpAddr, uint8_t EEPROMMqttPortAddr);
    void loopManagement(unsigned long actualTimeStamp, uint8_t EEPROMIntervalAddr);
}

#endif