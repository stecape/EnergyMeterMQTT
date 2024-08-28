#include "WebPage.h"
#include <EEPROM.h>

// Dimensione dell'indirizzo IP (4 byte)
#define IP_SIZE 4

void Web::setup(EthernetServer& ethServer, uint8_t EEPROMIpAddr){
  // Variabile per il mac address
  byte mac[] = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
  // Variabile per l'indirizzo IP
  byte ip[IP_SIZE];
  // Indirizzo IP di default
  byte defaultIP[IP_SIZE] = { 192, 168, 2, 2 };
  
  // Legge l'indirizzo IP dalla EEPROM
  bool ipValido = true;
  for (int i = 0; i < IP_SIZE; i++) {
    ip[i] = EEPROM.read(i+EEPROMIpAddr);
    if (ip[i] == 0xFF) { // Verifica se l'IP in EEPROM non è valorizzato
      ipValido = false;
    }
  }
  // Se l'IP non è valido, scrive l'indirizzo IP di default nella EEPROM
  if (!ipValido) {
    for (int i = 0; i < IP_SIZE; i++) {
      ip[i] = defaultIP[i];
      EEPROM.write(i+EEPROMIpAddr, defaultIP[i]);
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
  ethServer.begin();
}

void Web::loopManagement(EthernetClient client, uint8_t EEPROMIpAddr, uint8_t EEPROMMqttIpAddr, uint8_t EEPROMMqttPortAddr) {
  
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
        EEPROM.write(i, ipBytes[i]);
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
      Serial.println(body.c_str());  


      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println("<html><body><h1>IP Aggiornato</h1></body></html>");
      client.stop();

      Serial.println("Indirizzo IP aggiornato e salvato nella EEPROM. Riavvia per renderlo effettivo");
    } else {
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println("<html><body>");
      client.println("<h1>Imposta Indirizzo IP</h1>");
      client.println("<form method='POST' action='/set_ip'>");
      client.print("Byte 1: <input type='number' name='ip1' min='1' max='255' value='");
      client.print(EEPROM.read(EEPROMIpAddr));
      client.println("' required><br><br>");
      client.print("Byte 2: <input type='number' name='ip2' min='1' max='255' value='");
      client.print(EEPROM.read(EEPROMIpAddr+1));
      client.println("' required><br><br>");
      client.print("Byte 3: <input type='number' name='ip3' min='1' max='255' value='");
      client.print(EEPROM.read(EEPROMIpAddr+2));
      client.println("' required><br><br>");
      client.print("Byte 4: <input type='number' name='ip4' min='2' max='255' value='");
      client.print(EEPROM.read(EEPROMIpAddr+3));
      client.println("' required><br><br>");
      client.println("<button type='submit'>Invia</button>");
      client.println("</form>");
      client.println("</body></html>");
      client.stop();
    }
  }
}

void saveBin(int address, bool value) {
    EEPROM.put(address, value);
}

bool readBin(int address) {
    bool value;
    EEPROM.get(address, value);
    return value;
}

void saveInt(int address, int value) {
    EEPROM.put(address, value);
}

int readInt(int address) {
    int value;
    EEPROM.get(address, value);
    return value;
}

void saveFloat(int address, float value) {
    EEPROM.put(address, value);
}

float readFloat(int address) {
    float value;
    EEPROM.get(address, value);
    return value;
}

void saveString(int address, const char* value) {
    int len = strlen(value);
    EEPROM.write(address, len);
    for (int i = 0; i < len; i++) {
        EEPROM.write(address + 1 + i, value[i]);
    }
}

String readString(int address) {
    int len = EEPROM.read(address);
    char value[len + 1];
    for (int i = 0; i < len; i++) {
        value[i] = EEPROM.read(address + 1 + i);
    }
    value[len] = '\0';
    return String(value);
}