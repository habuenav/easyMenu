// FILE: basicTest.ino
// AUTHOR: Holman Buenaventura
// PURPOSE: navigate the menu using a rotary encoder.

#define LIBRARY GFX
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <easyMenu.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
easyMenu Menu(display);

#define PIN_CLK 2
#define PIN_DT  3
#define PIN_SW  4

volatile int encoderDirection = 0; // 1: Down, -1: Up
volatile bool buttonPressed = false;

void readEncoder() {
  static uint8_t lastCLK = HIGH;
  uint8_t currentCLK = digitalRead(PIN_CLK);
  
  if (currentCLK != lastCLK && currentCLK == LOW) {
    if (digitalRead(PIN_DT) != currentCLK) {
      encoderDirection = 1;  // Abajo
    } else {
      encoderDirection = -1; // Arriba
    }
  }
  lastCLK = currentCLK;
}

bool handleUp() {
  if (encoderDirection == -1) { encoderDirection = 0; return true; }
  return false;
}

bool handleDown() {
  if (encoderDirection == 1) { encoderDirection = 0; return true; }
  return false;
}

bool handleEnter() {
  static uint32_t lastPress = 0;
  if (digitalRead(PIN_SW) == LOW && (millis() - lastPress > 200)) {
    lastPress = millis();
    return true;
  }
  return false;
}

txtMenu menuEncoder[] = {"ENCODER ROTATIVO", "Ajustar Ganancia", "Filtro Pasa Bajo", "Frecuencia ", "Guardar "};

void setup() {
  pinMode(PIN_CLK, INPUT_PULLUP);
  pinMode(PIN_DT, INPUT_PULLUP);
  pinMode(PIN_SW, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PIN_CLK), readEncoder, CHANGE);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  Menu.attachUp(handleUp);
  Menu.attachDown(handleDown);
  Menu.attachEnter(handleEnter);

  Menu.assign(menuEncoder, "ENC_MENU");
  Menu.begin();
}

void loop() {
  Menu.show();

  if (Menu.enter()) {
    Menu.message("Opcion|Seleccionada!", CENTER, 1000);
  }
  updateMenu;
}
