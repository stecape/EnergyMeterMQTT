#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
//#include <SD.h>
#include "MQTT.h"
#include "WebPage.h"
#include <EEPROM.h>

///////Definizioni
// Dimensione dell'indirizzo IP (4 byte)
#define IP_SIZE 4
// Indirizzo in memoria EEPROM dell'IP
#define EEPROM_IP_ADDRESS 0
// Variabile per l'indirizzo IP
byte ip[IP_SIZE];

///////Inizializzazioni
// SD Card params
//const int chipSelect = 4;

// Actual timestamp
unsigned long actualTimeStamp = 0;
// Indirizzo IP di default
byte defaultIP[IP_SIZE] = { 192, 168, 2, 2 };

EthernetServer WebPageEthServer(80);
EthernetClient MQTTethClient;
PubSubClient MQTTclient(MQTTethClient);

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };

void setup() {

  Serial.begin(9600);
  
  // Inizializza la scheda SD
  // if (!SD.begin(chipSelect)) {
  //   Serial.println("Errore nell'inizializzazione della scheda SD!");
  //   return;
  // }




  // Legge l'indirizzo IP dalla EEPROM
  bool ipValido = true;
  for (int i = 0; i < IP_SIZE; i++) {
    ip[i] = EEPROM.read(i+EEPROM_IP_ADDRESS);
    if (ip[i] == 0xFF) { // Verifica se l'IP in EEPROM non è valorizzato
      ipValido = false;
    }
  }

  // Se l'IP non è valido, scrive l'indirizzo IP di default nella EEPROM
  if (!ipValido) {
    for (int i = 0; i < IP_SIZE; i++) {
      ip[i] = defaultIP[i];
      EEPROM.write(i+EEPROM_IP_ADDRESS, defaultIP[i]);
    }
    Serial.println("IP non valido in EEPROM, scritto IP di default.");
  } else {
    Serial.println("IP letto dalla EEPROM.");

    // Stampa l'indirizzo IP utilizzato
    Serial.print("Indirizzo IP: ");
    for (int i = 0; i < IP_SIZE; i++) {
      Serial.print(ip[i]);
      if (i < IP_SIZE - 1) {
        Serial.print(".");
      }
    }
    Serial.println();
  }




  Ethernet.begin(mac,ip);
  WebPageEthServer.begin();

  MQTT::setup(MQTTclient);

  // Allow the hardware to sort itself out
  delay(1500);
}

void loop() {
  actualTimeStamp = millis();
  MQTT::loopManagement(MQTTclient, actualTimeStamp, 10000);

  EthernetClient WebPageEthClient = WebPageEthServer.available();
  if (WebPageEthClient) {
      handleClient(WebPageEthClient, 0);
  }
}