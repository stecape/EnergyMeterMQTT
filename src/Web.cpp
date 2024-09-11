#include "Web.h"
#include "utilities.h"
#include "DEFINITION.h"

EthernetServer ethServer(80);

void Web::setup(){
  // Variabile per il mac address
  byte mac[] = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
  // Variabile per l'indirizzo IP
  byte ip[IP_SIZE];
  // Indirizzo IP di default
  byte defaultIP[IP_SIZE] = { 192, 168, 2, 2 };

  validateOrInitializeIP(dueFlashStorage, EEPROM_IP_ADDRESS, ip, defaultIP);

  Ethernet.begin(mac,ip);
  ethServer.begin();
}

void Web::loopManagement() {

  EthernetClient client = ethServer.available();
  
  if (client) {

    String request = client.readStringUntil('\r');
    client.flush();
    generateResponse(IP, client, dueFlashStorage, "IP", request, EEPROM_IP_ADDRESS);
    generateResponse(IP, client, dueFlashStorage, "IP MQTT Broker", request, EEPROM_MQTT_IP_ADDRESS);
    generateResponse(INTERO, client, dueFlashStorage, "Porta MQTT Broker", request, EEPROM_MQTT_PORT_ADDRESS);
    generateResponse(INTERO, client, dueFlashStorage, "Will QoS", request, EEPROM_MQTT_QOS_ADDRESS);
    generateResponse(INTERO, client, dueFlashStorage, "Refresh Rate", request, EEPROM_INTERVAL_ADDRESS);
    generateResponse(STRINGA, client, dueFlashStorage, "Client ID", request, EEPROM_CLIENT_ADDRESS);
    generateResponse(STRINGA, client, dueFlashStorage, "Will Topic", request, EEPROM_WILL_TOPIC_ADDRESS);
    generateResponse(STRINGA, client, dueFlashStorage, "Will Message", request, EEPROM_WILL_MESSAGE_ADDRESS);
    generateResponse(STRINGA, client, dueFlashStorage, "Topic", request, EEPROM_TOPIC_ADDRESS);
    generateResponse(STRINGA, client, dueFlashStorage, "User Name", request, EEPROM_USER_NAME_ADDRESS);
    generateResponse(STRINGA, client, dueFlashStorage, "Password", request, EEPROM_PASSWORD_ADDRESS);

    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println();
    client.println("<html><body>");
    
    generateForm(IP, dueFlashStorage, client, "Imposta Indirizzo IP", "IP", EEPROM_IP_ADDRESS, 0, 0);
    client.println("<br><br>");
    generateForm(IP, dueFlashStorage, client, "Imposta Indirizzo IP MQTT Broker", "IP MQTT Broker", EEPROM_MQTT_IP_ADDRESS, 0, 0);
    client.println("<br><br>");
    generateForm(INTERO, dueFlashStorage, client, "Imposta la porta dell'MQTT Broker", "Porta MQTT Broker", EEPROM_MQTT_PORT_ADDRESS, 1024, 49151);
    client.println("<br><br>");
    generateForm(INTERO, dueFlashStorage, client, "Imposta il QoS del Will", "Will QoS", EEPROM_MQTT_QOS_ADDRESS, 0, 2);
    client.println("<br><br>");
    generateForm(INTERO, dueFlashStorage, client, "Imposta il refresh rate del sensore", "Refresh Rate", EEPROM_INTERVAL_ADDRESS, 1, 60000);
    client.println("<br><br>");
    generateForm(STRINGA, dueFlashStorage, client, "Imposta il Client ID", "Client ID", EEPROM_CLIENT_ADDRESS, 0, 0);
    client.println("<br><br>");
    generateForm(STRINGA, dueFlashStorage, client, "Imposta il Will Topic", "Will Topic", EEPROM_WILL_TOPIC_ADDRESS, 0, 0);
    client.println("<br><br>");
    generateForm(STRINGA, dueFlashStorage, client, "Imposta il Will Message", "Will Message", EEPROM_WILL_TOPIC_ADDRESS, 0, 0);
    client.println("<br><br>");
    generateForm(STRINGA, dueFlashStorage, client, "Imposta il Topic", "Topic", EEPROM_TOPIC_ADDRESS, 0, 0);
    client.println("<br><br>");
    generateForm(STRINGA, dueFlashStorage, client, "Imposta lo User Name", "User Name", EEPROM_USER_NAME_ADDRESS, 0, 0);
    client.println("<br><br>");
    generateForm(STRINGA, dueFlashStorage, client, "Imposta la password", "Password", EEPROM_PASSWORD_ADDRESS, 0, 0);

    client.println("</body></html>");
    client.stop();
      
  }
}