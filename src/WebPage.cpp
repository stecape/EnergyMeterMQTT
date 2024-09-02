#include "WebPage.h"
#include <DueFlashStorage.h>

// Dimensione dell'indirizzo IP (4 byte)
#define IP_SIZE 4

enum TYPES {
  STRINGA,
  INTERO,
  IP
};

EthernetServer ethServer(80);

void Web::setup(DueFlashStorage dueFlashStorage, uint8_t EEPROMIpAddr){
  // Variabile per il mac address
  byte mac[] = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
  // Variabile per l'indirizzo IP
  byte ip[IP_SIZE];
  // Indirizzo IP di default
  byte defaultIP[IP_SIZE] = { 192, 168, 2, 2 };
  
  // Legge l'indirizzo IP dalla EEPROM
  bool ipValido = true;
  for (int i = 0; i < IP_SIZE; i++) {
    ip[i] = dueFlashStorage.read(i+EEPROMIpAddr);
    if (ip[i] == 0xFF) { // Verifica se l'IP in EEPROM non è valorizzato
      ipValido = false;
    }
  }
  // Se l'IP non è valido, scrive l'indirizzo IP di default nella EEPROM
  if (!ipValido) {
    for (int i = 0; i < IP_SIZE; i++) {
      ip[i] = defaultIP[i];
      dueFlashStorage.write(i+EEPROMIpAddr, defaultIP[i]);
    }
    ///Serial.println("IP non valido in EEPROM, scritto IP di default.");
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
  ethServer.begin();
}

void generateForm(TYPES t, DueFlashStorage dueFlashStorage, EthernetClient client, const String& title, const String& _varName, uint8_t EEPROMAddr, uint16_t min, uint16_t max){
  String varName = _varName;
  varName.replace(" ", "_");
  // Variabile di appoggio indirizzi in Flash
  byte* addr;
  client.print("<h1>");
  client.print(title);
  client.println("</h1>");
  client.print("<form method='POST' action='/");
  client.print(varName);
  client.println("'>");
  client.print(_varName);
  switch (t) {
    case STRINGA:
      Serial.println("Temperature is low");
      break;
    case INTERO:
      uint16_t variable;
      client.print("' (min: ");
      client.print(min);
      client.print("', max: ");
      client.print(max);
      client.print("): ");
      client.print(": <input type='number' name='");
      client.print(varName);
      client.print("' min='");
      client.print(min);
      client.print("' max='");
      client.print(max);
      client.print("' value='");
      addr = dueFlashStorage.readAddress(EEPROMAddr);
      memcpy(&variable, addr, sizeof(uint16_t));
      client.print(variable);
      break;
    case IP:
      client.print(": <input type='number' name='");
      client.print(varName);
      client.print("1' min='1' max='255' value='");
      client.print(dueFlashStorage.read(EEPROMAddr));
      client.print("' required>.");
      client.print("<input type='number' name='");
      client.print(varName);
      client.print("2' min='1' max='255' value='");
      client.print(dueFlashStorage.read(EEPROMAddr+1));
      client.print("' required>.");
      client.print("<input type='number' name='");
      client.print(varName);
      client.print("3' min='1' max='255' value='");
      client.print(dueFlashStorage.read(EEPROMAddr+2));
      client.print("' required>.");
      client.print("<input type='number' name='");
      client.print(varName);
      client.print("4' min='1' max='255' value='");
      client.print(dueFlashStorage.read(EEPROMAddr+3));
      break;
    default:
      Serial.println("Error");
      break;
  }
  client.println("' required><br><br>");
  client.println("<button type='submit'>Invia</button>");
  client.println("</form>");
}

void generateResponse(TYPES t){
  switch (t) {
    case STRINGA:
      Serial.println("Temperature is low");
      break;
    case INTERO:
      Serial.println("Temperature is medium");
      break;
    case IP:
      Serial.println("Temperature is high");
      break;
    default:
      Serial.println("Sensor error");
      break;
  }
}

void Web::loopManagement(DueFlashStorage dueFlashStorage, uint8_t EEPROMIpAddr, uint8_t EEPROMMqttIpAddr, uint8_t EEPROMMqttPortAddr, uint8_t EEPROMIntervalAddr) {

  // Array per leggere una unit16_t dalla Flash
  byte byteArray[sizeof(uint16_t)];
  // Porta dell'MQTT Broker
  uint16_t porta;
  // Refresh rate del sensore
  uint16_t intervallo;

  EthernetClient client = ethServer.available();
  
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("POST /IP ") != -1) {
      // Legge l'header della richiesta
      while (client.available()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          break;
        }
      }

      // Legge il corpo della richiesta
      String body = client.readStringUntil('\n');
      client.flush();

      int ipBytes[IP_SIZE];
      sscanf(body.c_str(), "IP1=%d&IP2=%d&IP3=%d&IP4=%d", &ipBytes[0], &ipBytes[1], &ipBytes[2], &ipBytes[3]);

      for (int i = 0; i < IP_SIZE; i++) {
        dueFlashStorage.write(i+EEPROMIpAddr, ipBytes[i]);
      }

      // Stampa l'indirizzo IP utilizzato
      Serial.print("Indirizzo IP: ");
      for (int i = 0; i < IP_SIZE; i++) {
        Serial.print(ipBytes[i]);
        if (i < IP_SIZE - 1) {
          Serial.print(".");
        }
      }
      Serial.println();

      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println("<html><body><h1>IP Aggiornato</h1></body></html>");
      client.stop();

      Serial.println("Indirizzo IP aggiornato e salvato nella EEPROM. Riavvia per renderlo effettivo");
    }
    else if (request.indexOf("POST /IP_MQTT_Broker") != -1) {
      // Legge l'header della richiesta
      while (client.available()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          break;
        }
      }

      // Legge il corpo della richiesta
      String body = client.readStringUntil('\n');
      client.flush();

      int ipBytes[IP_SIZE];
      sscanf(body.c_str(), "IP_MQTT_Broker1=%d&IP_MQTT_Broker2=%d&IP_MQTT_Broker3=%d&IP_MQTT_Broker4=%d", &ipBytes[0], &ipBytes[1], &ipBytes[2], &ipBytes[3]);

      for (int i = 0; i < IP_SIZE; i++) {
        dueFlashStorage.write(i+EEPROMMqttIpAddr, ipBytes[i]);
      }

      // Stampa l'indirizzo IP utilizzato
      Serial.print("Indirizzo IP MQTT Broker: ");
      for (int i = 0; i < IP_SIZE; i++) {
        Serial.print(ipBytes[i]);
        if (i < IP_SIZE - 1) {
          Serial.print(".");
        }
      }
      Serial.println();

      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println("<html><body><h1>IP MQTT Broker Aggiornato</h1></body></html>");
      client.stop();

      Serial.println("Indirizzo IP MQTT Broker aggiornato e salvato nella EEPROM. Riavvia per renderlo effettivo");
    }
    else if (request.indexOf("POST /Porta_MQTT_Broker") != -1) {
      // Legge l'header della richiesta
      while (client.available()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          break;
        }
      }

      // Legge il corpo della richiesta
      String body = client.readStringUntil('\n');
      client.flush();

      sscanf(body.c_str(), "Porta_MQTT_Broker=%hu", &porta);

      memcpy(byteArray, &porta, sizeof(uint16_t));
      dueFlashStorage.write(EEPROMMqttPortAddr, byteArray, sizeof(uint16_t));

      // Stampa la porta utilizzata
      Serial.print("MQTT Broker port: ");
      Serial.println(porta);

      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println("<html><body><h1>Porta MQTT Broker aggiornata</h1></body></html>");
      client.stop();

      Serial.println("Porta MQTT Broker aggiornata e salvata nella EEPROM. Riavvia per renderla effettiva");
    }
    else if (request.indexOf("POST /Refresh_Rate") != -1) {
      // Legge l'header della richiesta
      while (client.available()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
          break;
        }
      }

      // Legge il corpo della richiesta
      String body = client.readStringUntil('\n');
      client.flush();

      sscanf(body.c_str(), "Refresh_Rate=%hu", &intervallo);

      memcpy(byteArray, &intervallo, sizeof(uint16_t));
      dueFlashStorage.write(EEPROMIntervalAddr, byteArray, sizeof(uint16_t));

      // Stampa il refresh rate utilizzato
      Serial.print("Refresh rate: ");
      Serial.println(intervallo);

      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println("<html><body><h1>Refresh rate Aggiornato</h1></body></html>");
      client.stop();

      Serial.println("Refresh rate aggiornato e salvato nella EEPROM. Riavvia per renderlo effettivo");
    }
    else {
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
}