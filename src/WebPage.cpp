/*#include "WebPage.h"
#include <EEPROM.h>

void handleClient(EthernetClient client) {
    if (client) {
        String currentLine = "";
        String postData = "";
        bool isPost = false;
        Serial.println("Here");

        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                if (c == '\n') {
                    if (currentLine.length() == 0) {
                        if (isPost) {
                            int idx = postData.indexOf('=');
                            if (idx != -1) {
                                String key = postData.substring(0, idx);
                                String value = postData.substring(idx + 1);
                                saveString(0, value.c_str()); // Salva il valore all'indirizzo 0
                            }
                        }

                        File webFile = SD.open("index.html");
                        if (webFile) {
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-Type: text/html");
                            client.println("Connection: close");
                            client.println();
                            while (webFile.available()) {
                                client.write(webFile.read());
                            }
                            webFile.close();
                        } else {
                            client.println("HTTP/1.1 404 Not Found");
                            client.println("Content-Type: text/html");
                            client.println("Connection: close");
                            client.println();
                            client.println("<html><body><h1>404 Not Found</h1></body></html>");
                        }
                        break;
                    } else {
                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine += c;
                }

                if (currentLine.startsWith("POST")) {
                    isPost = true;
                }

                if (isPost && c == '\n' && currentLine.length() == 1) {
                    while (client.available()) {
                        char c = client.read();
                        postData += c;
                    }
                }
            }
        }
        delay(1);
        client.stop();
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
}*/

#include "WebPage.h"
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
          client.println("Connection: close");
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<form action=\"/\" method=\"POST\">");
          client.println("Stringa: <input type=\"text\" name=\"stringa\"><br>");
          client.println("<input type=\"submit\" value=\"Invia\">");
          client.println("</form>");
          client.println("</html>");
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
}