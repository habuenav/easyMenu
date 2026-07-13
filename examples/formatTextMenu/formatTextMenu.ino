#define LIBRARY GFX // 0 para usar U8g2lib, 1 para usar Adafruit_GFX
#include "Arduino.h"
#include <Wire.h>
#include <easyMenu.h> // Tu nueva versión de la librería

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C

#include <Adafruit_SSD1306.h> 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);  

easyMenu Menu(display);
  
txtMenu opciones[] = {
    "menu", // The first element acts as the Title if you set 'title = true'
    "Cap on",
    "Cap off",
    "tit left",
    "tit right",
    "item cen",
    "item left",
    "submenu"
};
txtMenu submenu[] = {
    "submenu",
    "opt a",
    "opt b",
    "opt c",
    "Exit"
};

bool buttomUp()    { if(digitalRead(12) == LOW) { delay(200); return true; } return false; }
bool buttomDown()  { if(digitalRead(14) == LOW) { delay(200); return true; } return false; }
bool buttomEnter() { if(digitalRead(27) == LOW) { delay(200); return true; } return false; }

void setup() {
    Serial.begin(115200);
    pinMode(12, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP); 
    pinMode(27, INPUT_PULLUP);
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {   Serial.println("SSD1306 allocation failed");  return;  }

    Menu.begin();
   
    Menu.setAutoOff(30)     // Turn off the screen after 30 seconds of inactivity
        .setInterline(5);      // Interline space 5 píxeles 
    //attach buttons for navigation
    Menu.attachUp(event(buttomUp()));
    Menu.attachDown(event(buttomDown()));
    Menu.attachEnter(event(buttomEnter()));
    Menu.Root(opciones, "PRINCIPAL", true); 
}

void loop() {
    
    Menu.Show(); 

     if (Menu.Enter()) 
    { 
       // Evaluation of the selection in the root menu ("PRINCIPAL")
        if (Menu.Select(1, "PRINCIPAL")) { 
            Menu.Message("Formato|ON", CENTER, 800);
            Menu.format = true;
            Menu.Assign(opciones, "PRINCIPAL", true);
        }
        else if (Menu.Select(2, "PRINCIPAL")) {
            Menu.Message("Formato|OFF", CENTER, 800);
            Menu.format = false;
            Menu.Assign(opciones, "PRINCIPAL", true);
        }
        else if (Menu.Select(3, "PRINCIPAL")) {
            Menu.Message("Titulo|Izq", CENTER, 800);
            Menu.alignTitle = LEFT;
            Menu.Assign(opciones, "PRINCIPAL", true);
        }
        else if (Menu.Select(4, "PRINCIPAL")) {
            Menu.Message("Titulo|Der", CENTER, 800);
            Menu.alignTitle = RIGHT;
            Menu.Assign(opciones, "PRINCIPAL", true);
        }
        else if (Menu.Select(5, "PRINCIPAL")) {
            Menu.Message("Items|Centro", CENTER, 800);
            Menu.alignItem = CENTER; 
            Menu.Assign(opciones, "PRINCIPAL", true);
        }
        else if (Menu.Select(6, "PRINCIPAL")) {
            Menu.Message("Items|Izq", CENTER, 800);
            Menu.alignItem = LEFT; 
            Menu.Assign(opciones, "PRINCIPAL", true);
        }
        else if (Menu.Select(7, "PRINCIPAL")) {
            Menu.Message("SubMenu", CENTER, 800);
            // We change to the submenu assigning the text identifier "SUBMENU"
            Menu.Assign(submenu, "SUBMENU", true);
        }
        
       // Evaluation of actions within the Submenu ("SUBMENU")
        else if (Menu.Select(1, "SUBMENU")) { /* Acción Opc A */ }
        else if (Menu.Select(2, "SUBMENU")) { /* Acción Opc B */ }
        else if (Menu.Select(3, "SUBMENU")) { /* Acción Opc C */ }
        else if (Menu.Select(4, "SUBMENU")) { // Ítem "salir"
            Menu.Message("Back to main menu...", CENTER, 800);
            Menu.Root(); // Returns to the "MAIN" menu or Root Menu
        }
    }
 updateMenu;    
}
