#### **[[ENGLISH]](#spanish)**

# easyMenu
`easyMenu` es una biblioteca ligera y eficiente para Arduino, diseñada para crear y gestionar menús multinivel de manera sencilla. Es ideal para proyectos de hardware embebido que utilizan pantallas OLED/LCD y requieren una interfaz limpia y fluida.
 está optimizada para consumir el mínimo espacio en memoria RAM, agregando cero fragmentación de memoria (Heap) durante ejecuciones prolongadas.

## ✨Características Destacadas

- **🎞️Marquesina Automática (Scroll):** Si el texto de una opción seleccionada es más largo que el ancho físico de la pantalla, el menú inicia un desplazamiento horizontal suave de forma automática tras una breve pausa para permitir su lectura completa.
- **🔄Interlineado Adaptativo Anti-Recorte:** Calcula matemáticamente el espaciado vertical óptimo basándose en la altura real del display y el número de ítems. Además, incluye un margen de seguridad estricto para evitar que la última opción se dibuje cortada.
- **↩Callbacks Basados en Eventos:** Permite enganchar las funciones de lectura de tus botones físicos directamente a la lógica de navegación interna mediante la macro `event()`.
- **⚡Zero RAM Overhead:** Usa arreglos estáticos de cadenas de texto (const char*) alojados en memoria Flash.
- **🌙Auto-Off configurable:** Apagado automático por tiempo de inactividad.
- **📱Soporte Multi-Driver :** Compatible con **U8g2lib** y con **Adafruit_GFX** mediante una directiva de precompilación (`#define LIBRARY`).

## Modificación Opcional Recomendada para Adafruit_GFX

Si has usado antes la librería Adafruit_GFX, habrás notado que por defecto los tamaños de texto solo aceptan números enteros (1, 2, 3), lo que a veces resulta en saltos de tamaño muy abruptos en pantallas pequeñas.
Para solucionar esto, incluimos una versión modificada de los archivos Adafruit_GFX.h y Adafruit_GFX.cpp en la carpeta optional/ del repositorio. Creditos https://github.com/wmarkow/Adafruit-GFX-Library/tree/1.10.7-text-size

### ¿Qué hace esta modificación?
Cambia el tipo de dato para la escala del texto de uint8_t a float. Esto te permite usar tamaños de fuente intermedios como 1.2, 1.5, 1.7, logrando un ajuste visual de los menús mucho más fino y legible.

### ¿Cómo instalarla?
Ve a la carpeta donde se instalan tus librerías de Arduino, Busca la carpeta Adafruit_GFX_Library -> Reemplaza sus archivos Adafruit_GFX.h y Adafruit_GFX.cpp por los que se encuentran en la carpeta optional/ de easyMenu.

Nota: Este paso es completamente opcional. easyMenu funcionará con la versión estándar de Adafruit_GFX, pero tendrás una mejor flexibilidad estética si la aplicas.

## 📋Requisitos

### 📟Hardware
- **Microcontroladores:** Diseñada y optimizada tanto para arquitecturas clásicas de 8 bits (ATmega328p/Arduino Uno, Nano, Mega) como para chips modernos de 32 bits (ESP32, RP2040, STM32).
- **Pantallas:** Displays OLED/LCD (ej. el clásico SSD1306 de 0.96'') operados a través de U8g2 o Adafruit_GFX.
- **Entradas:** Pulsadores físicos, encoders rotativos o sensores capacitivos.

### 💾Software
- **Arduino IDE**: Versión 1.8.10 o superior (compatible con Arduino IDE 2.x).

## 📦Instalación

### Opción 1: Desde el Gestor de Librerías de Arduino (Recomendado)
1. Abre el **Arduino IDE**.
2. Ve al menú **Herramientas** > **Gestionar bibliotecas...** (o presiona `Ctrl + Shift + I` / `Cmd + Shift + I`).
3. En el buscador, escribe **easyMenu**.
4. Selecciona la librería y haz clic en **Instalar**.

### Opción 2: Instalación Manual
1. Clona o descarga este repositorio como un archivo ZIP.
2. En el Arduino IDE, ve a **Programa** > **Incluir Librería** > **Añadir biblioteca .ZIP...** y selecciona el archivo descargado (o descomprímelo directamente en tu carpeta `sketchbook/libraries/`).
   
*Alternativamente, puedes extraer el contenido del archivo ZIP directamente dentro de la carpeta `libraries` de tu directorio de trabajo de Arduino.*

> **Nota:** Asegúrate de tener instalada la librería gráfica que vayas a utilizar (**U8g2** o **Adafruit_GFX_Library**).

### 📝Codigo de ejemplo
```

```
### 🌐Demostraciones en linea
pruebalo tu mismo en [WOKWI](https://wokwi.com/projects/469313717218921473)

## Métodos de la API (`easyMenu`)

### Inicialización y Configuración
- **`begin()`**: Inicializa el estado del menú, borra la pantalla y calcula el layout inicial. Debe llamarse dentro del `setup()`.
- **`setCursor(const String& symbol)`**: Configura la forma de resaltar la opción activa. 
  - `setCursor("")` o `setCursor("BOX")`: Utiliza el recuadro blanco e invierte el color del texto (modo por defecto).
  - `setCursor(">")`, `setCursor("*")`, `setCursor("-")`: Antepone el símbolo especificado delante de la opción seleccionada.
- **`setMaxLines(int8_t val)`**: Define un límite absoluto para la cantidad máxima de líneas que se renderizan simultáneamente en pantalla.
- **`setInterline(int8_t val)`**: Fuerza un valor fijo de interlineado (espacio en píxeles entre líneas) sustituyendo el cálculo adaptativo automático.
- **`setAutoOff(uint16_t timeout)`**: Establece un temporizador en segundos para apagar la pantalla si no se detecta actividad.
- **`getInterline()`**: Devuelve el valor del interlineado actual.

### Gestión de Contenido
- **`assign(T &opt, tipo id, bool title = true)`**: Asigna y formatea las opciones de tu array. `opt`: array de cadenas; `id`: identificador único del menú (numérico o texto); `title`: define si el primer elemento actúa como título fijo.
- **`root(T &opt, tipo id, bool title = true)`**: Registra el menú raíz principal del sistema.
- **`root()`**: Redirige instantáneamente al flujo del menú raíz guardado previamente.

### Control de Flujo y Renderizado
- **`show()`**: Procesa las lecturas de los callbacks, actualiza las animaciones de marquesina, verifica el apagado automático y renderiza el menú eficientemente sólo cuando detecta cambios.
- **`pause(bool opt)`**: Congela o reanuda el redibujado automático del menú.
- **`attachUp() / attachDown() / attachEnter() / attachRoot()`**: Vicula callbacks para la lectura de entradas físicas.

### Eventos y Selección
- **`enter()`**: Procesa el callback del botón de confirmación, refresca la actividad y retorna `true` si fue accionado.
- **`select(uint8_t sel, tipo id)`**: Retorna `true` si el cursor está ubicado en el índice `sel` del menú identificado por `id`.
- **`message(String _text, uint8_t align = LEFT, int wait = 2000)`**: Muestra un mensaje temporal. Permite textos multilínea utilizando el separador `|` y los centra automáticamente.

### Navegación Manual
- **`up()` / `down()`**: Mueve la selección una posición hacia arriba o abajo.
- **`fullUp()` / `fullDown()`**: Salta directamente a la primera o última opción seleccionable del menú.

## 🤝 Contribuciones y Apoyo
¡Las contribuciones, *issues* y sugerencias son más que bienvenidas! Si tienes ideas para mejorar la librería o encuentras algún problema🐛, no dudes en abrir un *Issue* o enviar un *Pull Request*.
Si `easyMenu` te ha sido de utilidad o te ayudó en tu proyecto, **considera darle una ⭐ al repositorio**. Es una excelente manera de apoyar el proyecto y ayuda a que otros desarrolladores en la comunidad de Arduino puedan encontrarlo.

## ✍🏼Autor
Desarrollado y mantenido por **Holman Buenaventura**.

#### **[[SPANISH]](#english)** 

# easyMenu
`easyMenu` is a lightweight and efficient library for Arduino, designed to easily create and manage multi-level menus. It is ideal for embedded hardware projects using OLED/LCD displays that require a clean and fluid interface.
It is optimized to consume minimal RAM, add zero memory fragmentation (Heap) during extended execution.

## ✨Key Features

- **🎞️Automatic Marquee (Scroll):** If the text of a selected option is longer than the physical width of the screen, the menu automatically initiates a smooth horizontal scroll after a brief pause to allow for complete reading.
- **🔄Adaptive Line Spacing Anti-Cropping:** Mathematically calculates the optimal vertical spacing based on the actual height of the display and the number of items. It also includes a strict safety margin to prevent the last option from being cropped.
- **↩Event-Based Callbacks:** Allows you to hook the reading functions of your physical buttons directly into the internal navigation logic using the `event()` macro.
- **⚡Zero RAM Overhead:** Uses static arrays of text strings (const char*) stored in Flash memory.
- **🌙Configurable Auto-Off:** Automatic shutdown after a period of inactivity.
- **📱Multi-Driver Support:** Compatible with **U8g2lib** and **Adafruit_GFX** via a pre-compilation directive (`#define LIBRARY`).

## Recommended Optional Modification for Adafruit_GFX

If you have used the Adafruit_GFX library before, you may have noticed that by default, text sizes only accept integers (1, 2, 3), which sometimes results in very abrupt size jumps on small screens.
To solve this, we've included a modified version of the Adafruit_GFX.h and Adafruit_GFX.cpp files in the optional/ folder of the repository. Credits https://github.com/wmarkow/Adafruit-GFX-Library/tree/1.10.7-text-size

### What does this modification do?
It changes the data type for the text scaling from uint8_t to float. This allows you to use intermediate font sizes like 1.2, 1.5, and 1.7, resulting in a much finer and more legible visual adjustment of the menus.

### How to install it?
Go to the folder where your Arduino libraries are installed. Find the Adafruit_GFX_Library folder. Replace its Adafruit_GFX.h and Adafruit_GFX.cpp files with the ones found in the optional/ folder of easyMenu.

Note: This step is completely optional. easyMenu will work with the standard version of Adafruit_GFX, but you'll have better aesthetic flexibility if you apply this modification.

## 📋Requirements

### 📟Hardware
- **Microcontrollers:** Designed and optimized for both classic 8-bit architectures (ATmega328p/Arduino Uno, Nano, Mega) and modern 32-bit chips (ESP32, RP2040, STM32).
- **Displays:** OLED/LCD displays (e.g., the classic 0.96" SSD1306) operated via U8g2 or Adafruit_GFX.
- **Inputs:** Physical pushbuttons, rotary encoders, or capacitive sensors.

### 💾Software
- **Arduino IDE:** Version 1.8.10 or higher (compatible with Arduino IDE 2.x).

## 📦Installation

1. Download the library file as a .ZIP archive from this GitHub repository.
2. Open your **Arduino IDE**, go to the **Sketch > Include Library > Add Library .ZIP...** menu.
3. Select the downloaded `.ZIP` file to complete the installation.
*Alternatively, you can extract the contents of the ZIP file directly into the `libraries` folder of your Arduino working directory.*

### 📝Example Code
```

```
### 🌐Online Demo
Try it yourself at [WOKWI](https://wokwi.com/projects/469313717218921473)

## API Methods (`easyMenu`)

### Initialization and Configuration
- **`begin()`**: Initializes the menu state, clears the screen, and calculates the initial layout. Must be called within `setup()`.
- `setCursor(const String& symbol)`: Configures how the active option is highlighted.
- `setCursor("")` or `setCursor("BOX")`: Uses a white box and inverts the text color (default mode).
- `setCursor(">")`, `setCursor("*")`, `setCursor("-")`: Places the specified symbol before the selected option.
- `setMaxLines(int8_t val)`: Defines an absolute limit for the maximum number of lines rendered simultaneously on the screen.
- `setInterline(int8_t val)`: Forces a fixed line spacing value (space in pixels between lines) overriding the automatic adaptive calculation.
- `setAutoOff(uint16_t timeout)`: Sets a timer in seconds to turn off the screen if no activity is detected. - `getInterline()`: Returns the current line spacing value.

### Content Management
- **`assign(T &opt, type id, bool title = true)`**: Assigns and formats the options in your array. `opt`: Array of strings; `id`: Unique menu identifier (numeric or text); `title`: Defines whether the first element acts as a fixed title.
- **`root(T &opt, type id, bool title = true)`**: Registers the system's main root menu.
- **`root()`**: Instantly redirects to the previously saved root menu flow.

### Flow Control and Rendering
- **`show()`**: Processes callback reads, updates marquee animations, checks for automatic shutdown, and efficiently renders the menu only when changes are detected.
- **`pause(bool opt)`**: Freezes or resumes automatic menu redrawing.
- **`attachUp() / attachDown() / attachEnter() / attachRoot()`**: Links callbacks for reading physical inputs.

### Events and Selection
- **`enter()`**: Processes the confirmation button callback, refreshes the activity, and returns `true` if it was pressed.
- **`select(uint8_t sel, type id)`**: Returns `true` if the cursor is located at the `sel` index of the menu identified by `id`.
- **`message(String _text, uint8_t align = LEFT, int wait = 2000)`**: Displays a temporary message. Allows multiline text using the `|` separator and automatically centers it.

### Manual Navigation
- **`up()` / `down()`**: Moves the selection one position up or down.
- **`fullUp()` / `fullDown()`**: Jumps directly to the first or last selectable option in the menu.

## 🤝 Contributions and Support
Contributions, issues, and suggestions are more than welcome! If you have ideas for improving the library or encounter a problem 🐛, feel free to open an issue or submit a pull request.
If `easyMenu` has been useful to you or has helped you in your project, **please consider giving the repository a star**. It's a great way to support the project and helps other developers in the Arduino community find it.

## ✍🏼Autor
Developed and maintained by **Holman Buenaventura**.
