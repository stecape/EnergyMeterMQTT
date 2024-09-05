#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>
#include <DueFlashStorage.h>

namespace MQTT{
    // Funzioni per inizializzare e gestire il client MQTT
    void setup();
    void loopManagement(unsigned long actualTimeStamp);
}

#endif