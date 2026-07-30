// FILE: addOptions.ino
// AUTHOR: Holman Buenaventura
// PURPOSE: dynamically add options to the menu.

#define LIBRARY U8g2
#include <U8g2lib.h>
#include <easyMenu.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
easyMenu Menu(u8g2);

txtMenu menuBase[] = {"DISPOSITIVOS", "Agregar nuevo"};
uint8_t contadorDispositivos = 0;

void setup() {
  u8g2.begin();
  Menu.assign(menuBase, "DYNAMIC");
  Menu.setCursor("*");
  Menu.begin();
}

void loop() {
  Menu.show();

  // Simulación: Cada 3 segundos se simula un clic para añadir un elemento nuevo
  static uint32_t timer = 0;
  if (millis() - timer > 3000) {
    timer = millis();
    contadorDispositivos++;
    
    String nuevoItem = "Sensor #" + String(contadorDispositivos);
    Menu.addOption(nuevoItem); // Expande dinámicamente el array en RAM
    
    if (contadorDispositivos >= 5) {
      Menu.message("Maximo de|sensores alcanzado", CENTER, 1500);
    }
  }
}
