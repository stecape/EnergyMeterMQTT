#include "utilities.h"

void validateOrInitializeInt(DueFlashStorage dueFlashStorage, uint8_t EEPROMAddr, uint16_t& value, uint16_t defaultValue) {
  bool valueValid = true;
  byte* addr = dueFlashStorage.readAddress(EEPROMAddr);
  memcpy(&value, addr, sizeof(uint16_t));
  
  if (value == 0xFFFF) { // Verifica se il valore in EEPROM non è valorizzato
    valueValid = false;
  }
  
  // Se il valore non è valido, scrive quello di default nella EEPROM
  if (!valueValid) {
    value = defaultValue; 
    byte byteArray[sizeof(uint16_t)];
    memcpy(byteArray, &value, sizeof(uint16_t));
    dueFlashStorage.write(EEPROMAddr, byteArray, sizeof(uint16_t));
    Serial.println("Valore non valido in EEPROM, scritto quello di default.");
  } else {
    Serial.println("Valore letto dalla EEPROM.");
    // Stampa il valore utilizzato
    Serial.print("Valore: ");
    Serial.println(value);
  }
}

void validateOrInitializeIP(DueFlashStorage dueFlashStorage, uint8_t EEPROMAddr, uint8_t* ip, uint8_t* defaultIP) {
  bool ipValido = true;

  // Legge l'indirizzo IP dalla EEPROM
  for (int i = 0; i < IP_SIZE; i++) {
    ip[i] = dueFlashStorage.read(i + EEPROMAddr);
    if (ip[i] == 0xFF) { // Verifica se l'IP in EEPROM non è valorizzato
      ipValido = false;
    }
  }

  // Se l'IP non è valido, scrive l'indirizzo IP di default nella EEPROM
  if (!ipValido) {
    for (int i = 0; i < IP_SIZE; i++) {
      ip[i] = defaultIP[i];
      dueFlashStorage.write(i + EEPROMAddr, defaultIP[i]);
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
      char stringValue[100]; // Assumendo una lunghezza massima di 100 caratteri
      client.print(": <input type='text' name='");
      client.print(varName);
      client.print("' value='");
      addr = dueFlashStorage.readAddress(EEPROMAddr);
      strncpy(stringValue, (char*)addr, sizeof(stringValue));
      stringValue[sizeof(stringValue) - 1] = '\0'; // Assicurarsi che la stringa sia null-terminated
      client.print(stringValue);
      client.print("' maxlength='100' required>");
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

void generateResponse(TYPES t, EthernetClient client, DueFlashStorage dueFlashStorage, String _varName, String request, int EEPROMAddr) {
  char formatString[100];
  String varName = _varName;
  varName.replace(" ", "_");
  if (request.indexOf("POST /" + varName + " ") != -1) {
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
    switch (t) {
      case STRINGA:
        char stringValue[100]; // Assumendo una lunghezza massima di 100 caratteri
        
        // Costruisci la stringa di formato dinamica
        sprintf(formatString, "%s=%%s", varName.c_str());
        sscanf(body.c_str(), formatString, stringValue);

        // Salva la stringa in EEPROM
        for (unsigned int i = 0; i < sizeof(stringValue); i++) {
          dueFlashStorage.write(EEPROMAddr + i, stringValue[i]);
          if (stringValue[i] == '\0') break; // Interrompe la scrittura se trova il terminatore null
        }

        // Stampa la variabile utilizzata
        Serial.print(_varName);
        Serial.print(": ");
        Serial.println(stringValue);

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        client.print("<html><body><h1>");
        client.print(_varName);
        client.println(" Updated</h1></body></html>");
        client.stop();
        Serial.print(_varName);
        Serial.println(" updated and saved on EEPROM. Reboot to apply");
        break;
      case INTERO:
        uint16_t intero;
        
        // Costruisci la stringa di formato dinamica
        sprintf(formatString, "%s=%%hu", varName.c_str());
        sscanf(body.c_str(), formatString, &intero);

        dueFlashStorage.write(EEPROMAddr, intero);

        // Stampa la variabile utilizzata
        Serial.print(_varName);
        Serial.print(": ");
        Serial.println(intero);

        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        client.print("<html><body><h1>");
        client.print(_varName);
        client.println(" Updated</h1></body></html>");
        client.stop();
        Serial.print(_varName);
        Serial.println(" updated and saved on EEPROM. Reboot to apply");
        break;
      case IP:
        int ipBytes[IP_SIZE];
            
        // Costruisci la stringa di formato dinamica
        sprintf(formatString, "%s1=%%d&%s2=%%d&%s3=%%d&%s4=%%d", varName.c_str(), varName.c_str(), varName.c_str(), varName.c_str());
        sscanf(body.c_str(), formatString, &ipBytes[0], &ipBytes[1], &ipBytes[2], &ipBytes[3]);

        for (int i = 0; i < IP_SIZE; i++) {
          dueFlashStorage.write(i + EEPROMAddr, ipBytes[i]);
        }

        // Stampa la variabile utilizzata
        Serial.print(_varName);
        Serial.print(": ");
        for (int i = 0; i < IP_SIZE; i++) {
          Serial.print(ipBytes[i]);
          if (i < IP_SIZE - 1) {
            Serial.print(".");
          }
        }
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        client.print("<html><body><h1>");
        client.print(_varName);
        client.println(" Updated</h1></body></html>");
        client.stop();
        Serial.print(_varName);
        Serial.println(" updated and saved on EEPROM. Reboot to apply");
        break;
      default:
        Serial.println("Sensor error");
        break;
    }
    
    Serial.println();
  }
}