#include "MQTT.h"
#include <DueFlashStorage.h>
#include <Ethernet.h>

// Dimensione dell'indirizzo IP (4 byte)
#define IP_SIZE 4
// Definizione del refresh rate del sensore
#define INTERVAL 10000

EthernetClient MQTTethClient;
PubSubClient MQTTclient(MQTTethClient);

// variabile per memorizzare l'ultimo momento in cui l'azione è stata eseguita
unsigned long prevTimeStamp = 0;

void MQTT::setup(DueFlashStorage dueFlashStorage, uint8_t EEPROMMqttIpAddr, uint8_t EEPROMMqttPortAddr) {
  // Variabile per l'indirizzo IP
  byte ip[IP_SIZE];
  // Indirizzo IP di default
  byte defaultIP[IP_SIZE] = { 192, 168, 2, 1 };
  // Porta del broker MQTT di default
  uint16_t MQTTBrokerDefaultPort = 1883;

  // Legge l'indirizzo IP Mqtt broker dalla EEPROM
  bool ipValido = true;
  for (int i = 0; i < IP_SIZE; i++) {
    ip[i] = dueFlashStorage.read(i+EEPROMMqttIpAddr);
    if (ip[i] == 0xFF) { // Verifica se l'IP Mqtt broker in EEPROM non è valorizzato
      ipValido = false;
    }
  }
  // Se l'IP Mqtt broker non è valido, scrive l'indirizzo IP di default nella EEPROM
  if (!ipValido) {
    for (int i = 0; i < IP_SIZE; i++) {
      ip[i] = defaultIP[i];
      dueFlashStorage.write(i+EEPROMMqttIpAddr, defaultIP[i]);
    }
    Serial.println("IP Mqtt broker non valido in EEPROM, scritto IP di default.");
  } else {
    Serial.println("IP Mqtt broker letto dalla EEPROM.");
    // Stampa l'indirizzo IP Mqtt broker utilizzato
    Serial.print("Indirizzo IP Mqtt broker: ");
    for (int i = 0; i < IP_SIZE; i++) {
      Serial.print(ip[i]);
      if (i < IP_SIZE - 1) {
        Serial.print(".");
      }
    }
    Serial.println();
  }

  // Legge la porta dell'Mqtt broker dalla EEPROM
  bool portaValida = true;
  uint16_t porta;
  byte byteArrayLettura[sizeof(uint16_t)];
  dueFlashStorage.readAddress(EEPROMMqttPortAddr);
  memcpy(&porta, byteArrayLettura, sizeof(uint16_t));
  if (porta == 0xFFFF) { // Verifica se la porta dell'Mqtt broker in EEPROM non è valorizzata
    portaValida = false;
  }
  // Se la porta dell'Mqtt broker non è valida, scrive la porta di default nella EEPROM
  if (!portaValida) {
    porta = MQTTBrokerDefaultPort;
    byte byteArray[sizeof(uint16_t)]; 
    memcpy(byteArray, &MQTTBrokerDefaultPort, sizeof(uint16_t));
    dueFlashStorage.write(EEPROMMqttPortAddr, byteArray, sizeof(uint16_t));
    Serial.println("Porta dell'Mqtt broker non valida in EEPROM, scritta porta di default.");
  } else {
    Serial.println("Porta dell'Mqtt broker letta dalla EEPROM.");
    // Stampa la porta dell'Mqtt broker utilizzata
    Serial.print("Porta dell'Mqtt broker: ");
    Serial.println(porta);
  }
  
  MQTTclient.setServer(ip, 1883);
}

// Funzione per riconnettere il client MQTT in caso di disconnessione
void reconnect(PubSubClient& client) {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("MQTTClientTest02")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// Funzione per riconnettere il client MQTT in caso di disconnessione
void MQTT::loopManagement(unsigned long actualTimeStamp, uint8_t EEPROMIntervalAddr) {
   if (!MQTTclient.connected()) {
    reconnect(MQTTclient);
  }
  MQTTclient.loop();
 
  
  if (MQTTclient.connected() && actualTimeStamp - prevTimeStamp >= INTERVAL) {
    prevTimeStamp = actualTimeStamp;
    MQTTclient.publish("current1","42");    
  }
}