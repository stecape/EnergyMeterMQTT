#include "MQTT.h"
#include <EEPROM.h>

// Dimensione dell'indirizzo IP (4 byte)
#define IP_SIZE 4

// variabile per memorizzare l'ultimo momento in cui l'azione è stata eseguita
unsigned long prevTimeStamp = 0;
// refresh rate del sensore
unsigned long interval;

void MQTT::setup(PubSubClient& client, uint8_t EEPROMMqttIpAddr, uint8_t EEPROMMqttPortAddr, uint8_t EEPROMIntervalAddr) {
  // Variabile per l'indirizzo IP
  byte ip[IP_SIZE];
  // Indirizzo IP di default
  byte defaultIP[IP_SIZE] = { 192, 168, 2, 1 };
  // Porta del broker MQTT di default
  uint16_t MQTTBrokerDefaultPort = 1883;
  // Intervallo di lettura del sensore di default
  unsigned long defaultInterval = 10000;

  // Legge l'indirizzo IP Mqtt broker dalla EEPROM
  bool ipValido = true;
  for (int i = 0; i < IP_SIZE; i++) {
    ip[i] = EEPROM.read(i+EEPROMMqttIpAddr);
    if (ip[i] == 0xFF) { // Verifica se l'IP Mqtt broker in EEPROM non è valorizzato
      ipValido = false;
    }
  }
  // Se l'IP Mqtt broker non è valido, scrive l'indirizzo IP di default nella EEPROM
  if (!ipValido) {
    for (int i = 0; i < IP_SIZE; i++) {
      ip[i] = defaultIP[i];
      EEPROM.write(i+EEPROMMqttIpAddr, defaultIP[i]);
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
  EEPROM.get(EEPROMMqttPortAddr, porta);
  if (porta == 0xFFFF) { // Verifica se la porta dell'Mqtt broker in EEPROM non è valorizzata
    portaValida = false;
  }
  // Se la porta dell'Mqtt broker non è valida, scrive la porta di default nella EEPROM
  if (!portaValida) {
      porta = MQTTBrokerDefaultPort;
      EEPROM.put(EEPROMMqttPortAddr, MQTTBrokerDefaultPort);
    Serial.println("Porta dell'Mqtt broker non valida in EEPROM, scritta porta di default.");
  } else {
    Serial.println("Porta dell'Mqtt broker letta dalla EEPROM.");
    // Stampa la porta dell'Mqtt broker utilizzata
    Serial.print("Porta dell'Mqtt broker: ");
    Serial.println(porta);
  }

  // Legge l'intervallo di lettura del sensore dalla EEPROM
  bool intervalloValido = true;
  EEPROM.get(EEPROMIntervalAddr, interval);
  if (interval == 0xFFFFFFFF || interval == 0) { // Verifica se l'intervallo di lettura del sensore in EEPROM non è valorizzato
    intervalloValido = false;
  }
  // Se l'intervallo di lettura del sensore non è valido, scrive quello di default nella EEPROM
  if (!intervalloValido) {
      interval = defaultInterval;
      EEPROM.put(EEPROMIntervalAddr, defaultInterval);
    Serial.println("Intervallo di lettura del sensore non valido in EEPROM, scritto quello di default.");
  } else {
    Serial.println("Intervallo di lettura del sensore letto dalla EEPROM.");
    // Stampa l'intervallo di lettura del sensore utilizzato
    Serial.print("Intervallo di lettura del sensore : ");
    Serial.println(interval);
  }

  client.setServer(ip, porta);
}

// Funzione per riconnettere il client MQTT in caso di disconnessione
void reconnect(PubSubClient& client) {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("MQTTClientTest01")) {
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
void MQTT::loopManagement(PubSubClient& client, unsigned long actualTimeStamp) {
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