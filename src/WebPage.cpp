#include "WebPage.h"
#include <DueFlashStorage.h>

// Dimensione dell'indirizzo IP (4 byte)
#define IP_SIZE 4

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

void Web::loopManagement(DueFlashStorage dueFlashStorage, uint8_t EEPROMIpAddr, uint8_t EEPROMMqttIpAddr, uint8_t EEPROMMqttPortAddr, uint8_t EEPROMIntervalAddr) {

  // Variabile di appoggio indirizzi in Flash
  byte* addr;
  // Array per leggere una unit16_t dalla Flash
  byte byteArray[sizeof(uint16_t)];
  // Refresh rate del sensore
  uint16_t intervallo;

  EthernetClient client = ethServer.available();
  
  if (client) {
    String request = client.readStringUntil('\r');
    client.flush();

    if (request.indexOf("POST /set_ip") != -1) {
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
      sscanf(body.c_str(), "ip1=%d&ip2=%d&ip3=%d&ip4=%d", &ipBytes[0], &ipBytes[1], &ipBytes[2], &ipBytes[3]);

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
    else if (request.indexOf("POST /set_mqtt__broker_ip") != -1) {
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
      sscanf(body.c_str(), "ipb1=%d&ipb2=%d&ipb3=%d&ipb4=%d", &ipBytes[0], &ipBytes[1], &ipBytes[2], &ipBytes[3]);

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
    else if (request.indexOf("POST /set_refresh_rate") != -1) {
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

      sscanf(body.c_str(), "rr=%hu", &intervallo);

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
      client.println("<h1>Imposta Indirizzo IP</h1>");
      client.println("<form method='POST' action='/set_ip'>");
      client.print("Byte 1: <input type='number' name='ip1' min='1' max='255' value='");
      client.print(dueFlashStorage.read(EEPROMIpAddr));
      client.println("' required><br><br>");
      client.print("Byte 2: <input type='number' name='ip2' min='1' max='255' value='");
      client.print(dueFlashStorage.read(EEPROMIpAddr+1));
      client.println("' required><br><br>");
      client.print("Byte 3: <input type='number' name='ip3' min='1' max='255' value='");
      client.print(dueFlashStorage.read(EEPROMIpAddr+2));
      client.println("' required><br><br>");
      client.print("Byte 4: <input type='number' name='ip4' min='2' max='255' value='");
      client.print(dueFlashStorage.read(EEPROMIpAddr+3));
      client.println("' required><br><br>");
      client.println("<button type='submit'>Invia</button>");
      client.println("</form>");
      client.println("<br><br>");
      client.println("<h1>Imposta Indirizzo IP MQTT Broker</h1>");
      client.println("<form method='POST' action='/set_mqtt_broker_ip'>");
      client.print("Byte 1: <input type='number' name='ipb1' min='1' max='255' value='");
      client.print(dueFlashStorage.read(EEPROMMqttIpAddr));
      client.println("' required><br><br>");
      client.print("Byte 2: <input type='number' name='ipb2' min='1' max='255' value='");
      client.print(dueFlashStorage.read(EEPROMMqttIpAddr+1));
      client.println("' required><br><br>");
      client.print("Byte 3: <input type='number' name='ipb3' min='1' max='255' value='");
      client.print(dueFlashStorage.read(EEPROMMqttIpAddr+2));
      client.println("' required><br><br>");
      client.print("Byte 4: <input type='number' name='ipb4' min='2' max='255' value='");
      client.print(dueFlashStorage.read(EEPROMMqttIpAddr+3));
      client.println("' required><br><br>");
      client.println("<button type='submit'>Invia</button>");
      client.println("</form>");
      client.println("<br><br>");
      client.println("<h1>Imposta il refresh rate del sensore</h1>");
      client.println("<form method='POST' action='/set_refresh_rate'>");
      client.print("Refresh rate [ms] (min 1, max 3600000): <input type='number' name='rr' min='1' max='3600000' value='");
      addr = dueFlashStorage.readAddress(EEPROMIntervalAddr);
      memcpy(&intervallo, addr, sizeof(uint16_t));
      client.print(intervallo);
      client.println("' required><br><br>");
      client.println("<button type='submit'>Invia</button>");
      client.println("</form>");
      client.println("</body></html>");
      client.stop();
    }
  }
}