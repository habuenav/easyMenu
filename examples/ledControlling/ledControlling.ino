// FILE: menuLeds.ino
// AUTHOR: Holman Buenaventura
// PURPOSE: Show use menu library for led controlling.
#define LIBRARY U8g2
#include <easyMenu.h>

const uint8_t LED1_PIN = 2; 
const uint8_t LED2_PIN = 4;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);
easyMenu Menu(display);

txtMenu mainMenu[] = {
    "LED OPTIONS", 
    "Set led 1",
    "Set led 2",
    "All leds off", 
    "All leds on"    
};
txtMenu subMenu1[] = {
    "LED 1", 
    "Set on",
    "Set off", 
    "Exit"
};
txtMenu subMenu2[] = {
    "LED 2", 
    "Set on",
    "Set off", 
    "Exit"
};

bool buttomUp()    { if(digitalRead(12) == LOW) { delay(200); return true; } return false; }
bool buttomDown()  { if(digitalRead(14) == LOW) { delay(200); return true; } return false; }
bool buttomEnter() { if(digitalRead(27) == LOW) { delay(200); return true; } return false; }

void setup() {
    pinMode(12, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP); 
    pinMode(27, INPUT_PULLUP);
    pinMode(LED1_PIN, OUTPUT);
    pinMode(LED2_PIN, OUTPUT);
    Menu.begin();
    Menu.setAutoOff(15);
    Menu.attachUp(event(buttomUp()));
    Menu.attachDown(event(buttomDown()));
    Menu.attachEnter(event(buttomEnter()));
    Menu.Root(mainMenu, "MAIN_MENU");
}

void loop() {
    Menu.Show();

    if (Menu.Enter()) 
    {   // Main menu logic
        // =================================================================
        if (Menu.Select(1, "MAIN_MENU")) {  //Access the first option in the main menu
            Menu.Assign(subMenu1, "SUB1");  //The first submenu is assigned to be displayed
        }
        else if (Menu.Select(2, "MAIN_MENU")) { //Access the second option in the main menu
            Menu.Assign(subMenu2, "SUB2");     //The second submenu is assigned to be displayed
        }
        else if (Menu.Select(3, "MAIN_MENU")) { //Access the third option in the main menu
            digitalWrite(LED1_PIN, LOW);
            digitalWrite(LED2_PIN, LOW);
            Menu.Message("SYSTEM|All leds|turned off", CENTER, 1500); //It displays a message that lasts 1.5 seconds.
            Menu.Root(); 
        }
        else if (Menu.Select(4, "MAIN_MENU")) {
            digitalWrite(LED1_PIN, HIGH);
            digitalWrite(LED2_PIN, HIGH);
            Menu.Message("SYSTEM|All leds|turned on", CENTER, 1500);
            Menu.Root();
        }
        // Submenu 1 logic
        // =================================================================
        else if (Menu.Select(1, "SUB1")) { //Access the first option in the submenu 1
            digitalWrite(LED1_PIN, HIGH);
            Menu.Message("LED 1|Status: ON", CENTER, 1200);
            Menu.Root(); 
        }
        else if (Menu.Select(2, "SUB1")) { //Access the second option in the submenu 1
            digitalWrite(LED1_PIN, LOW);
            Menu.Message("LED 1|Status: OFF", CENTER, 1200);
            Menu.Root();
        }
        else if (Menu.Select(3, "SUB1")) { //Access the third option in the submenu 1
            Menu.Root(); // Return to main menu or root menu
        }
        // Submenu 1 logic
        // =================================================================
        else if (Menu.Select(1, "SUB2")) { //Access the first option in the submenu 2
            digitalWrite(LED2_PIN, HIGH);
            Menu.Message("LED 2|Status: ON", CENTER, 1200);
            Menu.Root();
        }
        else if (Menu.Select(2, "SUB2")) {  //Access the second option in the submenu 2
            digitalWrite(LED2_PIN, LOW);
            Menu.Message("LED 2|Status: OFF", CENTER, 1200);
            Menu.Root();
        }
        else if (Menu.Select(3, "SUB2")) { //Access the third option in the submenu 1
            Menu.Root(); // Return to main menu or root menu
        }
    }
}
