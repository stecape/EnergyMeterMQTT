#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
//#include <SD.h>
#include "MQTT.h"
#include "WebPage.h"

const int chipSelect = 4;
EthernetServer WebPageEthServer(80);

unsigned long prevTimeStamp = 0;   // variabile per memorizzare l'ultimo momento in cui l'azione è stata eseguita
unsigned long interval = 10000;     // intervallo tra le azioni (10 secondi)

EthernetClient MQTTethClient;
PubSubClient MQTTclient(MQTTethClient);

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192, 168, 2, 2);

void setup() {

  Serial.begin(9600);
  
  // Inizializza la scheda SD
  // if (!SD.begin(chipSelect)) {
  //   Serial.println("Errore nell'inizializzazione della scheda SD!");
  //   return;
  // }

  Ethernet.begin(mac, ip);
  WebPageEthServer.begin();
  setupMQTT(MQTTclient);

  // Allow the hardware to sort itself out
  delay(1500);
}

void loop() {
  if (!MQTTclient.connected()) {
    reconnect(MQTTclient);
  }
  MQTTclient.loop();
 
 
  unsigned long actualTimeStamp = millis();
  
  if (MQTTclient.connected() && actualTimeStamp - prevTimeStamp >= interval) {
    prevTimeStamp = actualTimeStamp;
    String Variable = readString(0);
    MQTTclient.publish("current1",Variable.c_str());    
  }

  EthernetClient WebPageEthClient = WebPageEthServer.available();
  
  if (WebPageEthClient) {
      handleClient(WebPageEthClient);
  }
}