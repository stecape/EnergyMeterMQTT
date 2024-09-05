#include <Arduino.h>
#include "MQTT.h"
#include "Web.h"

///////Inizializzazioni
// Actual timestamp
unsigned long actualTimeStamp = 0;

void setup() {
  Serial.begin(9600);
  Serial.println();

  Web::setup();
  MQTT::setup();

  delay(1500);
}

void loop() {
  actualTimeStamp = millis();

  Web::loopManagement();
  MQTT::loopManagement(actualTimeStamp);
}