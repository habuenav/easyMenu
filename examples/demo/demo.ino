// FILE: demo.ino
// AUTHOR: Holman Buenaventura
// PURPOSE: It's a self-test that goes through the menu options.

#define LIBRARY GFX
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <easyMenu.h>

Adafruit_SSD1306 display(128, 64, &Wire, -1);
easyMenu Menu(display);

txtMenu menuRaiz[]  = {"DEMO KIOSKO", "Graficos", "Ajustes", "Acerca de "};
txtMenu subGraficos[] = {"GRAFICOS", "Onda Senoidal", "Espectro FFT", "Volver "};

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  
  Menu.root(menuRaiz, "ROOT");
  Menu.setCursor(">");
  Menu.setMaxLines(3);
  Menu.begin();
}

void loop() {
  Menu.show();

  static uint32_t stepTimer = 0;
  static uint8_t paso = 0;

  if (millis() - stepTimer > 1800) {
    stepTimer = millis();
    paso++;

    switch (paso) {
      case 1:
        Menu.down(); // Mueve al ítem 'Graficos'
        break;
      case 2:
        Menu.assign(subGraficos, "GRAFICOS"); // Entra a Submenú
        break;
      case 3:
        Menu.down(); // Navega dentro del submenú
        break;
      case 4:
        Menu.message("Cargando Espectro...|Por favor espere", CENTER, 2000);
        break;
      case 5:
        Menu.root(); // Vuelve al menú principal
        paso = 0;
        break;
    }
  }
  updateMenu;
}
