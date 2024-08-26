#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>
namespace MQTT{
    // Funzioni per inizializzare e gestire il client MQTT
    void setup(PubSubClient& client);
    void loopManagement(PubSubClient& client, unsigned long actualTimeStamp, unsigned long interval);
}
#endif