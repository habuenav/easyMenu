#define LIBRARY U8g2
#include <U8g2lib.h>
#include <easyMenu.h>

// Configuración de pantalla OLED SSD1306 128x64 I2C
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

easyMenu Menu(u8g2);

// Estructuras de menús usando el alias txtMenu
txtMenu menuPrincipal[] = {"AJUSTES", "Titulo", "Items", "Cursor", "Formato", "Salir"};
txtMenu subAlignTitle[] = {"ALINEAR", "Izq", "Centro", "Der", "Volver"};
txtMenu subAlignItem[] = {"ALINEAR", "Izq", "Centro", "Der", "Volver"};
txtMenu subCursor[]     = {"CURSOR", "' > '", "' * '", "' - '", "' ■ '", "Volver"};
txtMenu subFormat[]     = {"FORMATO", "May/Min(ON)", "May/Min(OFF)", "Volver"};

// Lectura del Monitor Serial para navegación por teclado
bool readSerialUp() {
  if (Serial.available() > 0) {
    char c = Serial.peek();
    if (c == 'w' || c == 'W') { Serial.read(); return true; }
  }
  return false;
}

bool readSerialDown() {
  if (Serial.available() > 0) {
    char c = Serial.peek();
    if (c == 's' || c == 'S') { Serial.read(); return true; }
  }
  return false;
}

bool readSerialEnter() {
  if (Serial.available() > 0) {
    char c = Serial.peek();
    if (c == 'e' || c == 'E' || c == '\n' || c == '\r') { Serial.read(); return true; }
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  u8g2.begin();

  // Asignar callbacks de control serial
  Menu.attachUp(readSerialUp);
  Menu.attachDown(readSerialDown);
  Menu.attachEnter(readSerialEnter);

  // Guardar raíz e iniciar menú
  Menu.root(menuPrincipal, "MAIN", true);
  Menu.setCursor(">");
  Menu.begin();

  Serial.println(F("--- Control de Menu por Consola Serial ---"));
  Serial.println(F("Usa 'w' (Arriba), 's' (Abajo) y 'e' o ENTER (Aceptar)"));
}

void loop() {
  Menu.show();

  // Gestión de Selección y Transición a Submenús
  if (Menu.enter()) {
    
    // --- MENÚ PRINCIPAL ---
    if (Menu.select(1, "MAIN")) {
      Menu.assign(subAlignTitle, "SUB_ALIGN_T");
    } 
    else if (Menu.select(2, "MAIN")) {
      Menu.assign(subAlignItem, "SUB_ALIGN_I");
    } 
    else if (Menu.select(3, "MAIN")) {
      Menu.assign(subCursor, "SUB_CURSOR");
    } 
    else if (Menu.select(4, "MAIN")) {
      Menu.assign(subFormat, "SUB_FORMAT");
    } 
    else if (Menu.select(5, "MAIN")) {
      Menu.message("Saliendo del|Menu...", CENTER, 1500);
    }

    // --- SUBMENÚ: ALINEAR TÍTULO ---
    else if (Menu.select(1, "SUB_ALIGN_T")) { Menu.alignTitle = LEFT;   Menu.root(); }
    else if (Menu.select(2, "SUB_ALIGN_T")) { Menu.alignTitle = CENTER; Menu.root(); }
    else if (Menu.select(3, "SUB_ALIGN_T")) { Menu.alignTitle = RIGHT;  Menu.root(); }
    else if (Menu.select(4, "SUB_ALIGN_T")) { Menu.root(); }

    // --- SUBMENÚ: ALINEAR ITEMS ---
    else if (Menu.select(1, "SUB_ALIGN_I"))  { Menu.alignItem = LEFT;   Menu.root(); }
    else if (Menu.select(2, "SUB_ALIGN_I"))  { Menu.alignItem = CENTER; Menu.root(); }
    else if (Menu.select(3, "SUB_ALIGN_I"))  { Menu.alignItem = RIGHT;  Menu.root(); }
    else if (Menu.select(4, "SUB_ALIGN_I"))  { Menu.root(); }

    // --- SUBMENÚ: ESTILO DE CURSOR ---
    else if (Menu.select(1, "SUB_CURSOR"))      { Menu.setCursor(">");   Menu.root(); }
    else if (Menu.select(2, "SUB_CURSOR"))      { Menu.setCursor("*");   Menu.root(); }
    else if (Menu.select(3, "SUB_CURSOR"))      { Menu.setCursor("-");   Menu.root(); }
    else if (Menu.select(4, "SUB_CURSOR"))      { Menu.setCursor("BOX"); Menu.root(); }
    else if (Menu.select(5, "SUB_CURSOR"))      { Menu.root(); }

    // --- SUBMENÚ: FORMATO DE TEXTO ---
    else if (Menu.select(1, "SUB_FORMAT"))      { Menu.format = true;  Menu.root(); }
    else if (Menu.select(2, "SUB_FORMAT"))      { Menu.format = false; Menu.root(); }
    else if (Menu.select(3, "SUB_FORMAT"))      { Menu.root(); }
  }
}
