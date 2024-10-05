#include "fabgl.h"

fabgl::VGA16Controller DisplayController;
fabgl::PS2Controller PS2Controller;
fabgl::Keyboard Keyboard;

const int screenWidth = 640;
const int screenHeight = 480;
const int blockSize = 10;
const int maxSnakeLength = 100;

// Pozice hada (pole segmentů)
int snakeX[maxSnakeLength] = {screenWidth / 2};
int snakeY[maxSnakeLength] = {screenHeight / 2};
int snakeLength = 1;

// Směr hada
int directionX = 0;
int directionY = 0;

// Pozice potravy
int foodX;
int foodY;

fabgl::Canvas cv;

void setup() {
  Serial.begin(115200);
  // Inicializace VGA ovladače
  DisplayController.begin();
  DisplayController.setResolution(VGA_640x480_60Hz);
  cv = fabgl::Canvas (&DisplayController);

  // Inicializace PS/2 klávesnice
  PS2Controller.begin(PS2Preset::KeyboardPort0);
  Keyboard.begin(true, true, 0);

  // Vytvoření počáteční potravy
  randomSeed(analogRead(0));
  foodX = random(0, screenWidth / blockSize) * blockSize;
  foodY = random(0, screenHeight / blockSize) * blockSize;

  // Vymazání obrazovky
  cv.setBrushColor(Color::Black);
  cv.clear();
}

void resetGame() {
  // Reset pozice hada
  snakeX[0] = screenWidth / 2;
  snakeY[0] = screenHeight / 2;
  snakeLength = 1;
  directionX = 0;
  directionY = 0;

  // Vytvoření nové potravy
  foodX = random(0, screenWidth / blockSize) * blockSize;
  foodY = random(0, screenHeight / blockSize) * blockSize;
  
}

void loop() {

  if (Keyboard.virtualKeyAvailable()) {
    auto key = Keyboard.getNextVirtualKey();
    if (key == fabgl::VK_UP && directionY == 0) {
      directionX = 0;
      directionY = -blockSize;
    } else if (key == fabgl::VK_DOWN && directionY == 0) {
      directionX = 0;
      directionY = blockSize;
    } else if (key == fabgl::VK_LEFT && directionX == 0) {
      directionX = -blockSize;
      directionY = 0;
    } else if (key == fabgl::VK_RIGHT && directionX == 0) {
      directionX = blockSize;
      directionY = 0;
    } else if (key == fabgl::VK_ESCAPE) {
      resetGame();
    }
  }


  // Aktualizace pozice hada
  for (int i = snakeLength - 1; i > 0; i--) {
    snakeX[i] = snakeX[i - 1];
    snakeY[i] = snakeY[i - 1];
  }
  snakeX[0] += directionX;
  snakeY[0] += directionY;

  if (snakeX[0]<0) snakeX[0] = screenWidth - blockSize;
  if (snakeY[0]<0) snakeY[0] = screenHeight - blockSize;
  if (snakeX[0]>screenWidth) snakeX[0] = 0;
  if (snakeY[0]>screenHeight) snakeY[0] = 0;

  // Kontrola kolize s jídlem
  if (snakeX[0] == foodX && snakeY[0] == foodY) {
    snakeLength++;
    foodX = random(0, screenWidth / blockSize) * blockSize;
    foodY = random(0, screenHeight / blockSize) * blockSize;
  }

  // Vymazání obrazovky
  cv.setBrushColor(Color::Black);
  cv.clear();

  // Vykreslení hada
  cv.setBrushColor(Color::Green);
  for (int i = 0; i < snakeLength; i++) {
    cv.fillRectangle(snakeX[i], snakeY[i], snakeX[i] + blockSize, snakeY[i] + blockSize);
  }

  // Vykreslení potravy
  cv.setBrushColor(Color::Red);
  cv.fillRectangle(foodX, foodY, foodX + blockSize, foodY + blockSize);

  // Zpoždění pro zpomalení hry
  delay(200);
}
