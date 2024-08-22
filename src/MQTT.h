#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

void callback(char* topic, byte* payload, unsigned int length);

void reconnect(PubSubClient client);
