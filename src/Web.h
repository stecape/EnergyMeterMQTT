#ifndef WEB_H
#define WEB_H

#include <Ethernet.h>
#include <DueFlashStorage.h>

namespace Web {
  // Funzioni per inizializzare e gestire la parte web
  void setup();
  void loopManagement();
}

#endif