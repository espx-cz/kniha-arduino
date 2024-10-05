#include <SPIFFS.h>

auto func = [] () {
  Serial.println("Lambda");
};

auto dbl = [] (int n) int { return n*2; };

void setup() {
  
  int x = 5;
  auto mult = [&] (int n) int { return n*x;};
  x=2;
  Serial.print(mult(5));

  Serial.begin(115200);
  // Inicializace SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Při připojení SPIFFS došlo k chybě");
    return;
  }

  // Otevření souboru pro čtení
  File file = SPIFFS.open("/package.json", "r");
  if (!file) {
    Serial.println("Otevření souboru pro čtení se nezdařilo");
    return;
  }

  Serial.println("Obsah souboru:");

  // Čtení z souboru a zaslání obsahu na sériový port
  while (file.available()) {
    Serial.print((char)file.read());
  }

  // Zavření souboru
  file.close();

  Serial.print(mult(5));
}

void loop() {
  // Zde nic není potřeba, veškerá funkčnost je v setup
}