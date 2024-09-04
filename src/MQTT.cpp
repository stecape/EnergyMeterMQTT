#include "MQTT.h"
#include <DueFlashStorage.h>
#include <Ethernet.h>
#include "utilities.h"

EthernetClient MQTTethClient;
PubSubClient MQTTclient(MQTTethClient);

// Refresh rate del sensore
uint16_t intervallo;
// Nome client
String clientName;
// Porta dell'Mqtt broker
uint16_t porta;

// variabile per memorizzare l'ultimo momento in cui l'azione è stata eseguita
unsigned long prevTimeStamp = 0;
unsigned long lastReconnectAttempt = 0;

void MQTT::setup(DueFlashStorage dueFlashStorage, uint8_t EEPROMMqttIpAddr, uint8_t EEPROMMqttPortAddr, uint8_t EEPROMIntervalAddr, uint8_t EEPROMclientNameAddr) {
  // Variabile per l'indirizzo IP
  byte ip[IP_SIZE];
  // Indirizzo IP di default
  byte defaultIP[IP_SIZE] = { 192, 168, 2, 1 };
  // Porta del broker MQTT di default
  uint16_t MQTTBrokerDefaultPort = 1883;
  // Refresh rate del sensore di default
  uint16_t defaultInterval = 10000;

  validateOrInitializeIP(dueFlashStorage, EEPROMMqttIpAddr, ip, defaultIP);
  validateOrInitializeInt(dueFlashStorage, EEPROMIntervalAddr, intervallo, defaultInterval);
  validateOrInitializeInt(dueFlashStorage, EEPROMMqttPortAddr, porta, MQTTBrokerDefaultPort);

  MQTTclient.setServer(ip, porta);

}

// Funzione per riconnettere il client MQTT in caso di disconnessione
void reconnect(PubSubClient& client, String clientName) {
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      Serial.print("Attempting MQTT connection...");
      if (client.connect(clientName.c_str())) {
        Serial.println("connected");
        lastReconnectAttempt = 0;
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
      }
    }
  }
}

// Funzione per riconnettere il client MQTT in caso di disconnessione
void MQTT::loopManagement(unsigned long actualTimeStamp, uint8_t EEPROMIntervalAddr) {
  if (!MQTTclient.connected()) {
    reconnect(MQTTclient, "MQTTClientTest02");
  } else {
    MQTTclient.loop();
  }
  
  if (MQTTclient.connected() && actualTimeStamp - prevTimeStamp >= intervallo) {
    prevTimeStamp = actualTimeStamp;
    MQTTclient.publish("current1","42");
  }
}