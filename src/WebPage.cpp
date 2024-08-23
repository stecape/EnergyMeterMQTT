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

void handleClient(EthernetClient client) {
    if (client) {
        String currentLine = "";
        String postData = "";
        bool isPost = false;

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
                                Serial.println("here!!");
                            }
                        }

                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-Type: text/html");
                        client.println("Connection: close");
                        client.println();
                        client.println("<!DOCTYPE HTML>");
                        client.println("<html>");
                        client.println("<form action=\"/\" method=\"POST\">");
                        client.println("Variable: <input type=\"text\" name=\"variable\"><br>");
                        client.println("<input type=\"submit\" value=\"Submit\">");
                        client.println("</form>");
                        client.println("</html>");

                        break;
                    } else {
                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine = c;
                    Serial.println(currentLine);
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
}