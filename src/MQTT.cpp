#include "MQTT.h"
#include <Ethernet.h>
#include <PubSubClient.h>

// Definisci l'indirizzo del broker MQTT
IPAddress MQTTserverIP(192, 168, 2, 1);
unsigned long prevTimeStamp = 0;   // variabile per memorizzare l'ultimo momento in cui l'azione è stata eseguita


// Funzione di callback per la gestione dei messaggi in arrivo
void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (unsigned int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

// Funzione per configurare il client MQTT
void MQTT::setup(PubSubClient& client) {
    client.setServer(MQTTserverIP, 1883);
    client.setCallback(callback);
}

// Funzione per riconnettere il client MQTT in caso di disconnessione
void reconnect(PubSubClient& client) {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("MQTTClientTest01")) {
            Serial.println("connected");
            client.subscribe("test/topic");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

// Funzione per riconnettere il client MQTT in caso di disconnessione
void MQTT::loopManagement(PubSubClient& client, unsigned long actualTimeStamp, unsigned long interval) {
  if (!client.connected()) {
    reconnect(client);
  }
  client.loop();
 
  
  if (client.connected() && actualTimeStamp - prevTimeStamp >= interval) {
    prevTimeStamp = actualTimeStamp;
    String Variable = String(random(0, 100));
    client.publish("current1",Variable.c_str());    
  }
}