
#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include "MQTT.h"
#include "WebPage.h"

const int chipSelect = 4;
EthernetServer ethServer(80);

unsigned long prevTimeStamp = 0;   // variabile per memorizzare l'ultimo momento in cui l'azione è stata eseguita
unsigned long interval = 10000;     // intervallo tra le azioni (10 secondi)

EthernetClient ethClient;
PubSubClient MQTTclient(ethClient);


// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress MQTTserverIP(192, 168, 2, 1);
IPAddress ip(192, 168, 2, 2);

void setup() {

  Serial.begin(9600);
  
  // Inizializza la scheda SD
  if (!SD.begin(chipSelect)) {
    Serial.println("Errore nell'inizializzazione della scheda SD!");
    return;
  }

  Ethernet.begin(mac, ip);
  ethServer.begin();
  
  MQTTclient.setServer(MQTTserverIP, 1883);
  MQTTclient.setCallback(callback);

  if (MQTTclient.connect("TestClient01")) {
    MQTTclient.subscribe("Sensors");
  }
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (!MQTTclient.connected()) {
    reconnect(MQTTclient);
  }

  unsigned long actualTimeStamp = millis();
  
  if (MQTTclient.connected() && actualTimeStamp - prevTimeStamp >= interval) {
    prevTimeStamp = actualTimeStamp;
    String Variable = readVariable(0);
    MQTTclient.publish("current1",Variable.c_str());    
  }

  
    ethClient = ethServer.available();
    if (ethClient) {
        handleClient(ethClient);
    }
}