#### **[[ENGLISH]](#spanish)**

# easyMenu
`easyMenu` es una biblioteca ligera para Arduino diseñada para crear y gestionar menús multinivel adaptativos de manera sencilla. Es ideal para proyectos de hardware embebido que utilizan pantallas OLED/LCD y requieren una interfaz limpia y fluida.

## Características Destacadas

- **Gestión Dinámica Inteligente:** Olvídate de los arrays rígidos. Permite inicializar el menú con un array estático y expandirlo en tiempo de ejecución añadiendo opciones al vuelo de forma segura mediante `realloc`.
- **Marquesina Automática (Scroll):** Si el texto de una opción seleccionada es más largo que el ancho físico de la pantalla, el menú inicia un desplazamiento horizontal suave de forma automática tras una breve pausa para permitir su lectura completa.
- **Interlineado Adaptativo Anti-Recorte:** Calcula matemáticamente el espaciado vertical óptimo basándose en la altura real del display y el número de ítems. Además, incluye un margen de seguridad estricto para evitar que la última opción se dibuje cortada.
- **Callbacks Basados en Eventos:** Permite enganchar las funciones de lectura de tus botones físicos directamente a la lógica de navegación interna mediante la macro `event()`.
- **Soporte Multi-Driver :** Compatible directamente tanto con **U8g2lib** como con **Adafruit_GFX** mediante una directiva de precompilación (`#define LIBRARY`).

## Requisitos

### Hardware
- **Microcontroladores:** Diseñada y optimizada tanto para arquitecturas clásicas de 8 bits (ATmega328p/Arduino Uno, Nano, Mega) como para chips modernos de 32 bits (ESP32, RP2040, STM32).
- **Pantallas:** Displays OLED/LCD (ej. el clásico SSD1306 de 0.96'') operados a través de U8g2 o Adafruit_GFX.
- **Entradas:** Pulsadores físicos, encoders rotativos o sensores capacitivos.

### Software
- **Arduino IDE**: Versión 1.8.10 o superior (compatible con Arduino IDE 2.x).

## Instalación

1. Descarga el archivo de la biblioteca como un archivo `.ZIP` desde este repositorio de GitHub.
2. Abre tu **Arduino IDE**, ve al menú **Programa (Sketch) > Incluir biblioteca > Añadir biblioteca .ZIP...**
3. Selecciona el archivo `.ZIP` descargado para completar la instalación.

*Alternativamente, puedes extraer el contenido del archivo ZIP directamente dentro de la carpeta `libraries` de tu directorio de trabajo de Arduino.*

### Codigo de ejemplo
```
#include <Breath.h>
#define OUTPUT_PIN 13
#define CLOCK_PIN 12
Breath breath(OUTPUT_PIN, CLOCK_PIN);

void setup() {
  Serial.begin(115200);
  breath.setResistance(3);  // Ajustar la resistencia de soplo (1 = mínima, 5 = máxima).
  breath.setMaxOut(127);    // Establecer el valor máximo de salida.
}

void loop() {
  int16_t breathValue = breath.read();
  Serial.println(breathValue);
  delay(100);
}
```

## Métodos de la API (`easyMenu`)

### Inicialización y Configuración
- **`begin()`**: Inicializa el estado del menú, borra la pantalla y calcula el layout inicial. Debe llamarse en el `setup()`.
- **`setAutoOff(uint16_t timeout)`**: Establece un temporizador en segundos para apagar automáticamente la pantalla si no se detecta actividad.
- **`setInterline(int8_t val)`**: Si se pasa un parametro fuerza un valor fijo de interlineado (píxeles de separación) sustituyendo al cálculo adaptativo.
- **`setMaxLines(int8_t val)`**: Limita la cantidad máxima de líneas de opciones que se renderizarán siemultáneamente en pantalla.
- **`getInterline()`**: Devuelve el valor actual de interlineado calculado o asignado.

### Gestión de Contenido y Navegación
- **`Assign(T &opt, tipo id, bool title = true)`**: Duplica y formatea las opciones de tu array en el Heap. `opt`: array de strings; `id`: identificador único del menú (numérico o texto); `title`: indica si el primer elemento actúa como título fijo del menú.
- **`Root(T &opt, tipo id, bool title = true)`**: Define por única vez el menú Raíz principal del sistema.
- **`Root()`**: Sobrecarga que redirige instantáneamente al flujo del menú raíz previamente guardado.
- **`addOption(String nuevaOpcion)`**: Inserta dinámicamente una nueva opción al final del menú actual en tiempo de ejecución, expandiendo la memoria de forma segura.

### Control de Flujo y Renderizado
- **`Show()`**: Procesa las lecturas de los callbacks de navegación, gestiona la animación de la marquesina, evalúa el auto-apagado y redibuja el menú en pantalla de forma autónoma y eficiente (solo cuando detecta cambios).
- **`Pause(bool opt)`**: Pausa temporalmente el redibujado automático en pantalla. Útil para congelar la interfaz al realizar impresiones manuales externas.
- **`attachUp() / attachDown() / attachEnter()`**: Adjunta los callbacks o funciones encargadas de leer los pines físicos de navegación.

### Eventos y Selección
- **`Enter()`**: Procesa el callback del botón de selección, refresca el temporizador de actividad y devuelve `true` si el botón fue legítimamente pulsado.
- **`Select(uint8_t sel, tipo id)`**: Retorna `true` si el cursor del usuario está posicionado exactamente sobre el índice `sel` del menú con identificador `id`.
- **`Message(String _text, uint8_t align = LEFT, int wait = 2000)`**: Despliega una pantalla de mensaje temporal. Soporta textos multilínea usando el caracter de tubería (`|`) y calcula de forma automática el centrado vertical y horizontal de cada fragmento.

### Desplazamiento Manual
- **`Up()` / `Down()`**: Desplaza el cursor una opción hacia arriba o hacia abajo.
- **`fullUp()` / `fullDown()`**: Salta inmediatamente al primer ítem seleccionable o al último ítem del menú actual.

## Contribuciones
¡Las contribuciones son bien recibidas! Si encuentras algún error o deseas proponer optimizaciones adicionales para `easyMenu`, por favor siéntete libre de enviar un *Pull Request* o abrir un *Issue* directamente en el repositorio.


#### **[[SPANISH]](#english)** 

# easyMenu
`easyMenu` is a lightweight Arduino library designed to easily create and manage adaptive multilevel menus. It's ideal for embedded hardware projects that use a display and require a clean and fluid interface.

## Key Features

- **Intelligent Dynamic Management:** Forget rigid arrays. Initialize the menu with a static array and expand it at runtime by safely adding options on the fly using `realloc`.

- **Automatic Scrolling:** If the text of a selected option is longer than the physical width of the screen, the menu automatically initiates a smooth horizontal scroll after a brief pause to allow for complete reading.

- **Anti-Cropping Adaptive Line Spacing:** Mathematically calculates the optimal vertical spacing based on the actual display height and the number of items. It also includes a strict safety margin to prevent the last option from being cropped.

- **Event-Based Callbacks:** Allows you to hook the reading functions of your physical buttons directly into the internal navigation logic using the `event()` macro.

- **Multi-Driver Support:** Directly compatible with both **U8g2lib** and **Adafruit_GFX** via a precompilation directive (`#define LIBRARY`).

## Requirements

### Hardware
- **Microcontrollers:** Designed and optimized for both classic 8-bit architectures (ATmega328p/Arduino Uno, Nano, Mega) and modern 32-bit chips (ESP32, RP2040, STM32).

- **Displays:** All those supported by U8g2 or Adafruit_GFX (e.g., the classic 0.96" SSD1306).

- **Inputs:** Physical pushbuttons, rotary encoders, or capacitive sensors.

### Software
- **Arduino IDE**: Version 1.8.10 or higher (compatible with Arduino IDE 2.x).

## Installation

1. Download the library file as a .ZIP archive from this GitHub repository.

2. Open your **Arduino IDE**, go to the **Sketch > Include Library > Add Library .ZIP...** menu.

3. Select the downloaded .ZIP file to complete the installation.

*Alternatively, you can extract the contents of the ZIP file directly into the `libraries` folder of your Arduino working directory.*

## API Methods (`easyMenu`)

### Initialization and Configuration
- **`begin()`**: Initializes the menu state, clears the screen, and calculates the initial layout. It must be called in `setup()`.
- `setAutoOff(uint16_t timeout)`: Sets a timer in seconds to automatically turn off the screen if no activity is detected.
- `setInterline(int8_t val)`: If passed as a parameter, forces a fixed line spacing value (pixels of separation) overriding the adaptive calculation.
- `setMaxLines(int8_t val)`: Limits the maximum number of option lines that will be rendered simultaneously on the screen.
- `getInterline()`: Returns the current calculated or assigned line spacing value.

### Content and Navigation Management
- `Assign(T &opt, type id, bool title = true)`: Duplicates and formats the options in your array on the heap. `opt`: Array of strings; `id`: Unique identifier of the menu (numeric or text); `title`: Indicates whether the first item acts as the fixed menu title.
- **`Root(T &opt, tipo id, bool title = true)`**: Defines the system's main Root menu once.
- **`Root()`**: Overload that instantly redirects to the previously saved root menu flow.
- **`addOption(String nuevaOpcion)`**: Dynamically inserts a new option at the end of the current menu at runtime, safely expanding memory.

### Flow Control and Rendering
- **`Show()`**: Processes navigation callback readings, manages marquee animation, evaluates auto-shutdown, and automatically and efficiently redraws the menu on screen (only when changes are detected).
- **`Pause(bool opt)`**: Temporarily pauses automatic screen redrawing. Useful for freezing the interface when performing manual external print jobs.
- **`attachUp() / attachDown() / attachEnter()`**: Attaches the callbacks or functions responsible for reading the physical navigation pins.

### Events and Selection
- **`Enter()`**: Processes the selection button callback, refreshes the activity timer, and returns `true` if the button was legitimately pressed.
- **`Select(uint8_t sel, type id)`**: Returns `true` if the user's cursor is positioned exactly over the `sel` index of the menu with identifier `id`.
- **`Message(String _text, uint8_t align = LEFT, int wait = 2000)`**: Displays a temporary message screen. It supports multiline text using the pipe character (`|`) and automatically calculates the vertical and horizontal centering of each fragment.

### Manual Scrolling
- **`Up()` / `Down()`**: Moves the cursor one position up or down.
- **`fullUp()` / `fullDown()`**: Jump immediately to the first selectable item or the last item in the current menu.

## Autor
Desarrollado y mantenido por **Holman Buenaventura**.
