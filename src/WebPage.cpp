#include "WebPage.h"
#include <DueFlashStorage.h>
#include "utilities.h"

EthernetServer ethServer(80);

void Web::setup(DueFlashStorage dueFlashStorage, uint8_t EEPROMIpAddr){
  // Variabile per il mac address
  byte mac[] = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
  // Variabile per l'indirizzo IP
  byte ip[IP_SIZE];
  // Indirizzo IP di default
  byte defaultIP[IP_SIZE] = { 192, 168, 2, 2 };

  validateOrInitializeIP(dueFlashStorage, EEPROMIpAddr, ip, defaultIP);

  Ethernet.begin(mac,ip);
  ethServer.begin();
}

void Web::loopManagement(DueFlashStorage dueFlashStorage, uint8_t EEPROMIpAddr, uint8_t EEPROMMqttIpAddr, uint8_t EEPROMMqttPortAddr, uint8_t EEPROMIntervalAddr) {

  EthernetClient client = ethServer.available();
  
  if (client) {

    String request = client.readStringUntil('\r');
    client.flush();
    generateResponse(IP, client, dueFlashStorage, "IP", request, EEPROMIpAddr);
    generateResponse(IP, client, dueFlashStorage, "IP MQTT Broker", request, EEPROMMqttIpAddr);
    generateResponse(INTERO, client, dueFlashStorage, "Porta MQTT Broker", request, EEPROMMqttPortAddr);
    generateResponse(INTERO, client, dueFlashStorage, "Refresh Rate", request, EEPROMIntervalAddr);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<html><body>");
    
    generateForm(IP, dueFlashStorage, client, "Imposta Indirizzo IP", "IP", EEPROMIpAddr, 0, 0);
    client.println("<br><br>");
    generateForm(IP, dueFlashStorage, client, "Imposta Indirizzo IP MQTT Broker", "IP MQTT Broker", EEPROMMqttIpAddr, 0, 0);
    client.println("<br><br>");
    generateForm(INTERO, dueFlashStorage, client, "Imposta la porta dell'MQTT Broker", "Porta MQTT Broker", EEPROMMqttPortAddr, 1024, 49151);
    client.println("<br><br>");
    generateForm(INTERO, dueFlashStorage, client, "Imposta il refresh rate del sensore", "Refresh Rate", EEPROMIntervalAddr, 1, 60000);

    client.println("</body></html>");
    client.stop();
      
  }
}