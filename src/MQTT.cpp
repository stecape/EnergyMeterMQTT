#include "MQTT.h"
#include <DueFlashStorage.h>
#include <Ethernet.h>
#include "utilities.h"
#include "DEFINITION.h"

EthernetClient MQTTethClient;
PubSubClient MQTTclient(MQTTethClient);

float misuraLuminosita() {
  int valore_ldr = analogRead(A0);
  float val_ldr_convertito = map(valore_ldr, 747, 1024, 0, 100);

  return val_ldr_convertito;
}

int sensorValue = 0;
int accumulatedValue = 0;
int sampleCount = 0;

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
// will della connessione MQTT
String willTopic;
String willMessage;
uint16_t willQoS;
boolean willRetain;
// Clean session
boolean cleanSession = true;

// variabile per memorizzare l'ultimo momento in cui l'azione è stata eseguita
unsigned long prevTimeStamp = 0;
unsigned long lastReconnectAttempt = 0;

void MQTT::setup() {
  // Variabile per l'indirizzo IP
  byte ip[IP_SIZE];
  // Indirizzo IP di default
  byte defaultIP[IP_SIZE] = { 192, 168, 2, 151 };
  // Porta del broker MQTT di default
  uint16_t MQTTBrokerDefaultPort = 1883;
  // QoS della connessione MQTT di default
  uint16_t MQTTDefaultQoS = 0;
  // Refresh rate del sensore di default
  uint16_t defaultInterval = 10000;
  // Refresh rate del sensore di default
  String defaultTopic = "homeassistant/LightSensor1";
  // Default client name
  String defaultClientName = "LightSensor1";
  // Default userName e password
  String defaultUserName = "mqttuser";
  String defaultPassword = "mqttpassword";
  // Default will
  String defaultWillTopic = "";
  bool defaultWillRetain = false;
  String defaultWillMessage = "";

  validateOrInitializeIP(dueFlashStorage, EEPROM_MQTT_IP_ADDRESS, ip, defaultIP);
  validateOrInitializeInt(dueFlashStorage, EEPROM_INTERVAL_ADDRESS, intervallo, defaultInterval);
  validateOrInitializeInt(dueFlashStorage, EEPROM_MQTT_PORT_ADDRESS, porta, MQTTBrokerDefaultPort);
  validateOrInitializeInt(dueFlashStorage, EEPROM_MQTT_QOS_ADDRESS, willQoS, MQTTDefaultQoS);
  validateOrInitializeString(dueFlashStorage, EEPROM_TOPIC_ADDRESS, topic, defaultTopic, false);
  validateOrInitializeString(dueFlashStorage, EEPROM_CLIENT_ADDRESS, clientName, defaultClientName, false);
  validateOrInitializeString(dueFlashStorage, EEPROM_USER_NAME_ADDRESS, userName, defaultUserName, true);
  validateOrInitializeString(dueFlashStorage, EEPROM_PASSWORD_ADDRESS, password, defaultPassword, true);
  validateOrInitializeString(dueFlashStorage, EEPROM_WILL_TOPIC_ADDRESS, willTopic, defaultWillTopic, false);
  validateOrInitializeString(dueFlashStorage, EEPROM_WILL_MESSAGE_ADDRESS, willMessage, defaultWillMessage, false);
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
      const char *user;
      const char *pass;
      const char* willTp;
      const char* willMsg;

      if (userName.length() > 0 && password.length() > 0) {
        user = userName.c_str();
        pass = password.c_str();
      } else {
        user = NULL;
        pass = NULL;
      }
      if (willTopic.length() > 0 && willMessage.length() > 0) {
        willTp = willTopic.c_str();
        willMsg = willMessage.c_str();
      } else {
        willTp = 0;
        willMsg = 0;
        willQoS = 0;
        willRetain = false;
      }
      clientConnected = client.connect(clientName.c_str(), user, pass, willTp, willQoS, willRetain, willMsg, cleanSession);

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
    // Campiona il valore del sensore
  sensorValue = misuraLuminosita(); // Supponendo che il sensore sia collegato al pin A0
  accumulatedValue += sensorValue;
  sampleCount++;
  if (MQTTclient.connected() && actualTimeStamp - prevTimeStamp >= intervallo) {
    prevTimeStamp = actualTimeStamp;
    // Calcola il valore medio del sensore
    String averageValue = String(accumulatedValue / sampleCount);
    MQTTclient.publish(topic.c_str(), averageValue.c_str());
    accumulatedValue = 0;
    sampleCount = 0;
  }
}