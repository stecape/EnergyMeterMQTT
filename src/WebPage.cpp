/* #include "WebPage.h"
#include <EEPROM.h>

String data = "";

void handleClient(EthernetClient client) {
  if (client) {
    Serial.println("Nuovo client");
    boolean currentLineIsBlank = true;
    char buffer[300];
    int index = 0;
    boolean isPost = false;
    int contentLength = 0;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        // Salva i dati del form nel buffer
        if (index < sizeof(buffer) - 1) {
          buffer[index++] = c;
        }

        // Controlla se è una richiesta POST
        if (strncmp(buffer, "POST", 4) == 0) {
          isPost = true;
        }

        // Trova la lunghezza del contenuto
        if (strstr(buffer, "Content-Length:") != NULL) {
          char *cl = strstr(buffer, "Content-Length:") + 15;
          contentLength = atoi(cl);
        }

        // Invia la pagina web al client per le richieste GET
        if (c == '\n' && currentLineIsBlank) {
          // Invia la pagina web al client
      client.println("HTTP/1.1 200 OK");
      client.println("Content-Type: text/html");
      client.println();
      client.println("<html><body>");
      client.println("<h1>Imposta Indirizzo IP</h1>");
      client.println("<form method='POST' action='/set_ip'>");
      client.print("Byte 1: <input type='number' name='ip1' min='0' max='255' value='");
      client.print(ip[0]);
      client.println("' required><br><br>");
      client.print("Byte 2: <input type='number' name='ip2' min='0' max='255' value='");
      client.print(ip[1]);
      client.println("' required><br><br>");
      client.print("Byte 3: <input type='number' name='ip3' min='0' max='255' value='");
      client.print(ip[2]);
      client.println("' required><br><br>");
      client.print("Byte 4: <input type='number' name='ip4' min='0' max='255' value='");
      client.print(ip[3]);
      client.println("' required><br><br>");
      client.println("<button type='submit'>Invia</button>");
      client.println("</form>");
      client.println("</body></html>");
          break;
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }

    // Legge il corpo della richiesta POST
    if (isPost && contentLength > 0) {
      index = 0;
      while (client.available() && index < contentLength) {
        char c = client.read();
        if (index < sizeof(buffer) - 1) {
          buffer[index++] = c;
        }
      }
      buffer[index] = 0;

      // Analizza i dati del form POST
      char *start = strstr(buffer, "stringa=");
      if (start) {
        start += 8; // Salta "stringa="
        char *end = strchr(start, '&');
        if (!end) {
          end = strchr(start, ' ');
        }
        if (end) {
          *end = 0;
        }
        Serial.print("Stringa ricevuta: ");
        Serial.println(start);
        saveString(0, start); // Salva il valore all'indirizzo 0
      } else {
        Serial.println("Stringa non trovata nel buffer");
      }
    }

    // Resetta il buffer e le variabili per la prossima richiesta
    memset(buffer, 0, sizeof(buffer));
    index = 0;
    isPost = false;
    contentLength = 0;

    // Chiude la connessione
    delay(1);
    client.stop();
    Serial.println("Client disconnesso");
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
} */

#include "WebPage.h"
#include <EEPROM.h>

// Dimensione dell'indirizzo IP (4 byte)
#define IP_SIZE 4

void handleClient(EthernetClient client, uint8_t EEPROMipAddr) {
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
      client.print(EEPROM.read(EEPROMipAddr));
      client.println("' required><br><br>");
      client.print("Byte 2: <input type='number' name='ip2' min='1' max='255' value='");
      client.print(EEPROM.read(EEPROMipAddr+1));
      client.println("' required><br><br>");
      client.print("Byte 3: <input type='number' name='ip3' min='1' max='255' value='");
      client.print(EEPROM.read(EEPROMipAddr+2));
      client.println("' required><br><br>");
      client.print("Byte 4: <input type='number' name='ip4' min='2' max='255' value='");
      client.print(EEPROM.read(EEPROMipAddr+3));
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