
#include <Arduino.h>
#include "MQTT.h"
#include "EmonLib.h"

EnergyMonitor emon1;

unsigned long prevTimeStamp = 0;   // variabile per memorizzare l'ultimo momento in cui l'azione è stata eseguita
unsigned long interval = 10000;     // intervallo tra le azioni (10 secondi)

EthernetClient ethClient;
PubSubClient client(ethClient);


// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress server(192, 168, 2, 1);
IPAddress ip(192, 168, 2, 2);

void setup() {

  Serial.begin(9600);
  Ethernet.begin(mac, ip);

  emon1.current(0, 111.1);             // Current: input pin, calibration.
  
  client.setServer(server, 1883);
  client.setCallback(callback);

  if (client.connect("arduinoClient")) {
    client.publish("current1","0");
    client.subscribe("stecapeSensors");
  }
  // Allow the hardware to sort itself out
  delay(1500);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (!client.connected()) {
    reconnect(client);
  }

  unsigned long actualTimeStamp = millis();
  
  if (actualTimeStamp - prevTimeStamp >= interval) {
    prevTimeStamp = actualTimeStamp;
    double Irms = emon1.calcIrms(1480);  // Calculate Irms only
    client.publish("current1",String(Irms).c_str());
    
  }
}