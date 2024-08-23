#ifndef MQTT_H
#define MQTT_H

#include <PubSubClient.h>

// Funzioni per inizializzare e gestire il client MQTT
void setupMQTT(PubSubClient& client);
void reconnect(PubSubClient& client);
void callback(char* topic, byte* payload, unsigned int length);

#endif