#include "fabgl.h"

// Vytvoření instance VGA ovladače
fabgl::VGA16Controller DisplayController;

void setup() {
  // Inicializace VGA ovladače
  DisplayController.begin();

  // Nastavení rozlišení obrazovky
  DisplayController.setResolution(VGA_640x480_60Hz);

  fabgl::Canvas cv(&DisplayController);

  // Vymazání obrazovky
  cv.setBrushColor(Color::Yellow);
  cv.clear();

  // Nastavení barvy na červenou
  cv.setBrushColor(Color::Red);

  // Vykreslení obdélníku
  cv.fillRectangle(100, 100, 200, 150);
}

void loop() {
  // Smyčka je prázdná, protože vše probíhá v setupu
}
