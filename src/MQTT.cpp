#include "MQTT.h"
#include <DueFlashStorage.h>
#include <Ethernet.h>
#include "utilities.h"
#include "DEFINITION.h"

EthernetClient MQTTethClient;
PubSubClient MQTTclient(MQTTethClient);

// Refresh rate del sensore
uint16_t intervallo;
// Nome client
String clientName;
// Topic
String topic;
// Porta dell'Mqtt broker
uint16_t porta;
// username e password
String userName;
String password;

// variabile per memorizzare l'ultimo momento in cui l'azione è stata eseguita
unsigned long prevTimeStamp = 0;
unsigned long lastReconnectAttempt = 0;

void MQTT::setup() {
  // Variabile per l'indirizzo IP
  byte ip[IP_SIZE];
  // Indirizzo IP di default
  byte defaultIP[IP_SIZE] = { 192, 168, 2, 1 };
  // Porta del broker MQTT di default
  uint16_t MQTTBrokerDefaultPort = 1883;
  // Refresh rate del sensore di default
  uint16_t defaultInterval = 10000;
  // Refresh rate del sensore di default
  String defaultTopic = "homeassistant/Topic1";
  // Default client name
  String defaultClientName = "Client1";
  // Default userName e password
  String defaultUserName = "mqttuser";
  String defaultPassword = "mqttpassword";

  validateOrInitializeIP(dueFlashStorage, EEPROM_MQTT_IP_ADDRESS, ip, defaultIP);
  validateOrInitializeInt(dueFlashStorage, EEPROM_INTERVAL_ADDRESS, intervallo, defaultInterval);
  validateOrInitializeInt(dueFlashStorage, EEPROM_MQTT_PORT_ADDRESS, porta, MQTTBrokerDefaultPort);
  validateOrInitializeString(dueFlashStorage, EEPROM_TOPIC_ADDRESS, topic, defaultTopic, false);
  validateOrInitializeString(dueFlashStorage, EEPROM_CLIENT_ADDRESS, clientName, defaultClientName, false);
  validateOrInitializeString(dueFlashStorage, EEPROM_USER_NAME_ADDRESS, userName, defaultUserName, true);
  validateOrInitializeString(dueFlashStorage, EEPROM_PASSWORD_ADDRESS, password, defaultPassword, true);
  MQTTclient.setServer(ip, porta);
}

// Funzione per riconnettere il client MQTT in caso di disconnessione
void reconnect(PubSubClient& client, String clientName) {
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      Serial.print("Attempting MQTT connection...");

      bool clientConnected = false;
      if (userName.length() > 0 && password.length() > 0) {
        clientConnected = client.connect(clientName.c_str(), userName.c_str(), password.c_str());
      } else {
        clientConnected = client.connect(clientName.c_str());
      }

      if (clientConnected) {
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
void MQTT::loopManagement(unsigned long actualTimeStamp) {
  if (!MQTTclient.connected()) {
    reconnect(MQTTclient, clientName);
  } else {
    MQTTclient.loop();
  }
  
  if (MQTTclient.connected() && actualTimeStamp - prevTimeStamp >= intervallo) {
    prevTimeStamp = actualTimeStamp;
    MQTTclient.publish(topic.c_str(),"42");
  }
}