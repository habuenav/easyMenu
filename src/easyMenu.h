#ifndef easyMenu_h
#define easyMenu_h

#define U8g2 0 
#define GFX 1
#define event(btn_expr) []() { return btn_expr; }
#define NUMITEMS(arg) ((unsigned int) (sizeof (arg) / sizeof (arg [0])))
#include "Arduino.h"

#if LIBRARY == GFX
    #define updateMenu display.display(); 
#else
    #define updateMenu
#endif

typedef String txtMenu;
typedef bool (*MenuEventCallback)();

// Función isNumeric optimizada
inline bool isNumeric(String str) {   char* p;   return str.length() > 0 && (strtol(str.c_str(), &p, 10), *p == '\0'); }


template<typename T>
uint8_t size(T (&arr)) { 
    uint8_t n = NUMITEMS(arr);
    n = (n > 0) ? n - 1 : n;  
    if(arr[n][strlen(arr[n].c_str()) - 1] != ' ' && n > 0) { 
        arr[n] = String(arr[n] + ' ');  
    } else {  
        n = 0; 
        do { n++; } while(arr[n][strlen(arr[n].c_str()) - 1] != ' ');  
    }
    return n + 1;    
}

// Procesamiento de cadenas con '|' (Optimizado con punteros)
inline char *getStringLineStart(uint8_t line_idx, char *str) {
    uint8_t line_cnt = 1;
    if (line_idx == 0) return str;
    while (*str != '\0') {
        if (*str++ == '|') {
            if (line_cnt == line_idx) return str;
            line_cnt++;
        }
    }
    return NULL;
}

inline uint8_t stringLineCnt(char *str) {
    uint8_t line_cnt = 1;
    if (str == NULL) return 0;
    while (*str != '\0') {
        if (*str++ == '|') line_cnt++;
    }
    return line_cnt;
}

inline void copyStringLine(char *dest, uint8_t line_idx, char *str) {
    if (dest == NULL) return;
    str = getStringLineStart(line_idx, str);
    if (str != NULL) {
        while (*str != '|' && *str != '\0') {
            *dest++ = *str++;
        }
    }
    *dest = '\0';
}

// Configuración de Drivers de Pantalla
#if LIBRARY == U8g2
    #include <U8g2lib.h>
    #define LCDWidth disp.getDisplayWidth()
    #define LCDHeight disp.getDisplayHeight()
    #define STRHeight disp.getMaxCharHeight()
    #define STRWidth(t) disp.getStrWidth(t)
    #define CENTER_H(t) ((LCDWidth - STRWidth(t)) / 2)
    #define RIGHT_H(t) (LCDWidth - STRWidth(t))
#elif LIBRARY == GFX
    #include <Adafruit_GFX.h>
    #define LCDWidth disp.width()
    #define LCDHeight disp.height()
    #define STRHeight(t) strHeight(t)
    #define STRWidth(t) strWidth(t)
    #define CENTER_H(t) ((LCDWidth - STRWidth(t)) / 2)
    #define RIGHT_H(t) (LCDWidth - STRWidth(t))
    #define tx 2.4
    #define ty 1.9
    #define ix 2.1
    #define iy 1.9
#endif

enum { LEFT, CENTER, RIGHT };

class easyMenu {
public:
    easyMenu& AttachUp(MenuEventCallback cb) { _upCallback = cb; return *this; }
    easyMenu& AttachDown(MenuEventCallback cb) { _downCallback = cb; return *this; }
    easyMenu& AttachSelect(MenuEventCallback cb) { _selectCallback = cb; return *this; }

#if LIBRARY == U8g2
    easyMenu(U8G2& dispPointer) : disp(dispPointer) {
        if (!titleFont && !itemFont) {
            titleFont = itemFont = (LCDWidth * LCDHeight <= 4096) ? u8g2_font_roentgen_nbp_tr : u8g2_font_9x15B_tr;
        }
        if (!titleFont) titleFont = itemFont;
        if (!itemFont) itemFont = titleFont;
    }
    const uint8_t *titleFont = NULL;
    const uint8_t *itemFont = NULL;
#elif LIBRARY == GFX
    easyMenu(Adafruit_GFX& dispPointer) : disp(dispPointer) {}
#endif

    // Destructor (Libera memoria dinámica con free)
    ~easyMenu() {
        if (_opt) free(_opt);
        if (_root_opt) free(_root_opt);
    }

    void begin() {
        #if LIBRARY == GFX
        disp.fillScreen(0);
        #else
        disp.begin();
        #endif
        calculateLayout();
        _lastActivity = millis();
        _isOff = false;
    }

    bool PutSelect() {
        if (_selectCallback && _selectCallback()) {
            _lastActivity = millis();
            _isOff = false;
            return true;
        }
        return false;
    }

    void AddOption(String nuevaOpcion) {
        if (format) {
            nuevaOpcion.toLowerCase();
            if (nuevaOpcion.length() > 0) nuevaOpcion[0] = toUpperCase(nuevaOpcion[0]);
        }

        // El nuevo tamaño será la cantidad actual más la nueva ranura
        uint8_t nuevoTamano = _nroItems + 2; 

        // Reasignamos memoria dinámicamente usando realloc
        txtMenu* ptr_temporal = (txtMenu*) realloc(_opt, nuevoTamano * sizeof(txtMenu));

        if (ptr_temporal != nullptr) {
            _opt = ptr_temporal;
            _opt[nuevoTamano - 1] = nuevaOpcion;
            _nroItems = nuevoTamano - 1; 
            
            _change = true;      
            calculateLayout();   
        }
    }

    void Show() {
        if (_upCallback && _upCallback()) Up();
        if (_downCallback && _downCallback()) Down();
        checkAutoOff();
        if (_isOff || !(_change && !_pause)) return;

        #if LIBRARY == U8g2
        disp.clearBuffer();
        disp.setFontPosTop();
        if (_title) {
            disp.setFont(titleFont);
            disp.setCursor((alignTitle == CENTER) ? CENTER_H(_opt[0].c_str()) : (alignTitle == RIGHT) ? RIGHT_H(_opt[0].c_str()) : 2, 0);
            disp.print(_opt[0].c_str());
        }
        disp.setFont(itemFont);
        #elif LIBRARY == GFX
        disp.fillScreen(0);
        if (_title) {
            disp.setTextSize(tx, ty);
            disp.setTextColor(1);
            disp.setCursor((alignTitle == CENTER) ? CENTER_H(_opt[0]) : (alignTitle == RIGHT) ? RIGHT_H(_opt[0]) : 2, 0);
            disp.print(_opt[0]);
        }
        disp.setTextSize(ix, iy);
        #endif

        uint8_t initLine = (_selected <= _max_lines) ? _title : (_selected - _max_lines) + _title;
        for (uint8_t i = initLine; i <= _nroItems; i++) {
            uint8_t j = (_selected > _max_lines) ? (i - initLine) + _title : i;
            
            #if LIBRARY == U8g2
            uint8_t cur_x = (alignItem == CENTER) ? CENTER_H(_opt[i].c_str()) : (alignItem == RIGHT) ? RIGHT_H(_opt[i].c_str()) : 2;
            disp.setCursor(cur_x, (_lineht * j) + _interline);
            if (i == _selected) {
                disp.drawBox(cur_x, (_lineht * j) + _interline, STRWidth(_opt[i].c_str()) + 4, _lineht - 2);
                disp.setDrawColor(0);
                disp.print(_opt[i].c_str());
                disp.setDrawColor(1);
            } else {
                disp.print(_opt[i].c_str());
            }
            #elif LIBRARY == GFX
            disp.setCursor((alignItem == CENTER) ? CENTER_H(_opt[i]) : (alignItem == RIGHT) ? RIGHT_H(_opt[i]) : 2, j * (_lineht + _interline));
            disp.setTextColor(i == _selected ? 0 : 1, i == _selected ? 1 : 0);
            disp.print(_opt[i]);
            #endif
        }

        #if LIBRARY == U8g2
        disp.sendBuffer();
        #endif
       _change = false;
    }

    easyMenu& SetAutoOff(uint16_t timeout) { _timeout = timeout * 1000; _lastActivity = millis(); _isOff = false; return *this; }
    easyMenu& SetInterline(int8_t val) { interline = val; _change = true; return *this; }
    easyMenu& SetMaxLines(int8_t val) { max_lines = val; _change = true; return *this; }

    void Up() { handleNav((_selected <= _title) ? _nroItems : _selected - 1); }
    void Down() { handleNav((_selected < _nroItems) ? _selected + 1 : _title); }
    void fullUp() { handleNav(_title ? 1 : 0); }
    void fullDown() { handleNav(_nroItems); }

    void Message(String _text, uint8_t align = LEFT, int wait = 2000) {
        _lastActivity = millis();
        _isOff = false;
        char text[_text.length() + 1];
        _text.toCharArray(text, _text.length() + 1);
        uint8_t total_lines = stringLineCnt(text);
        uint8_t cur_x = 2;

        #if LIBRARY == U8g2
        disp.setFont(itemFont);
        disp.setFontPosTop();
        disp.clearBuffer();
        uint16_t font_height = STRHeight;
        #elif LIBRARY == GFX
        disp.setTextWrap(false);
        disp.fillScreen(0);
        disp.setTextColor(1);
        disp.setTextSize(ix, iy);
        uint16_t font_height = STRHeight("a");
        #endif

        char buf[LCDWidth / (STRWidth(text) / total_lines)];
        uint16_t total_height = total_lines * font_height;

        for (uint8_t i = 0; i < total_lines; i++) {
            copyStringLine(buf, i, text);
            if (align == CENTER || align == RIGHT) cur_x = (align == CENTER) ? CENTER_H(buf) : RIGHT_H(buf);
            disp.setCursor(cur_x, ((LCDHeight - total_height) / 2) + (i * font_height));
            disp.print(buf);
        }

        #if LIBRARY == U8g2
        disp.sendBuffer();
        #endif
        delay(wait);
        _change = true;
    }

    template <typename T, class tipo>
    void Assign(T &opt, tipo id, bool title = true) {
        allocateAndFormat(_opt, opt, id, title);
        _nroItems = size(opt) - 1;
        _title = title;
        _selected = title ? 1 : 0;
        calculateLayout();
    }

    template <typename T, class tipo>
    void Root(T &opt, tipo id, bool title = true) {
        allocateAndFormat(_root_opt, opt, id, title, true);
        _title = title;
        Assign(_root_opt, id, title);
    }

    void Root() {
        if (_root_id != -1) Assign(_root_opt, _root_id, _title);
        else if (_root_Id != "") Assign(_root_opt, _root_Id, _title);
    }

    template <class tipo>
    bool Select(uint8_t sel, tipo id) {
        _lastActivity = millis();
        _isOff = false;
        String test = String(id);
        return isNumeric(test) ? (test.toInt() == _id && _selected == sel) : (test == _Id && _selected == sel);
    }

    void Pause(bool opt) { _pause = opt; }
    int8_t GetInterline() { return _interline; }

    bool format = true;
    uint8_t alignTitle = LEFT;
    uint8_t alignItem = LEFT;
    int8_t interline = -1;
    int8_t max_lines = -1;
    uint8_t _selected = 0;

private:
    MenuEventCallback _upCallback = nullptr;
    MenuEventCallback _downCallback = nullptr;
    MenuEventCallback _selectCallback = nullptr;

#if LIBRARY == GFX
    uint16_t strWidth(String str) { int16_t x1, y1; uint16_t w, h; disp.getTextBounds(str.c_str(), disp.getCursorX(), disp.getCursorY(), &x1, &y1, &w, &h); return w; }
    uint16_t strHeight(String str) { int16_t x1, y1; uint16_t w, h; disp.getTextBounds(str.c_str(), disp.getCursorX(), disp.getCursorY(), &x1, &y1, &w, &h); return h; }
#endif

    void calculateLayout() {
        #if LIBRARY == U8g2
            disp.setFont(titleFont); 
            _titleht = _title ? STRHeight : 0; 
            disp.setFont(itemFont); 
            _lineht = STRHeight;
        #elif LIBRARY == GFX
            disp.setTextSize(tx, ty); 
            _titleht = _title ? STRHeight("A") : 0; 
            disp.setTextSize(ix, iy); 
            _lineht = STRHeight("A");
        #endif

        int16_t availableHeight = LCDHeight - _titleht;
        if (availableHeight < 0) availableHeight = 0;

        uint8_t max_possible_lines = availableHeight / _lineht;
        
        if (max_lines == -1) {
            _max_lines = max_possible_lines;
            if (interline != -1 && _max_lines > 0) {
                while ((_max_lines * _lineht + (_max_lines - 1) * interline) > availableHeight) {
                    _max_lines--;
                }
            }
        } else {
            _max_lines = max_lines;
        }

        if (interline != -1) {   _interline = interline; }
        else 
           {  uint8_t actual_visible_lines = (_nroItems < _max_lines) ? _nroItems : _max_lines;
              if (actual_visible_lines > 0) 
               { int16_t remainingPixels = availableHeight - (actual_visible_lines * _lineht);
                 uint8_t total_gaps = (actual_visible_lines - 1) + _title;
                  _interline= (total_gaps > 0) ? remainingPixels / total_gaps:  remainingPixels;
                  if (_interline > _lineht) {   _interline = _lineht; }
               } 
              else { _interline = 0; }
           }
    }

    void checkAutoOff() {
        if (_timeout > 0 && !_isOff && (millis() - _lastActivity > _timeout)) {
            #if LIBRARY == U8g2
            disp.clear();
            #elif LIBRARY == GFX
            disp.fillScreen(0);
            #endif
            _isOff = true;
        }
    }

    void handleNav(uint8_t newSel) { _change = true; _lastActivity = millis(); _isOff = false; _selected = newSel; }

    // Helper de asignación clásico migrado a malloc/free
    template <typename T, class tipo>
    void allocateAndFormat(txtMenu* &dest, T (&opt), tipo id, bool title, bool isRoot = false) {
        if (dest) free(dest);
        uint8_t tam = size(opt);
        String test = String(id);
        if (isRoot) {
            if (isNumeric(test)) _root_id = test.toInt(); else _root_Id = test; }
            else { if (isNumeric(test)) _id = test.toInt(); else _Id = test;    }
        
        dest = (txtMenu*) malloc(tam * sizeof(txtMenu));
        for (uint8_t i = 0; i < tam; i++) {
            // Inicialización segura del String en el bloque de memoria cruda
            new (&dest[i]) String(opt[i]); 
            if (format) {
                dest[i].toLowerCase();
                if (dest[i].length() > 0) dest[i][0] = toUpperCase(dest[i][0]);
                if (title && i == 0) dest[0].toUpperCase();
            }
        }
        _change = true; _isOff = false; _lastActivity = millis();
    }

#if LIBRARY == U8g2
    U8G2& disp;
#elif LIBRARY == GFX
    Adafruit_GFX& disp;
#endif

    txtMenu *_opt = nullptr;
    txtMenu *_root_opt = nullptr;
    String _Id;
    int _id = -1;
    String _root_Id;
    int _root_id = -1;
    bool _title = true;
    uint8_t _nroItems = 0;
    uint8_t _max_lines = 0;
    uint8_t _titleht = 0;
    uint8_t _lineht = 0;
    int8_t _interline = 0;
    uint32_t _timeout = 0;
    uint32_t _lastActivity = 0;
    bool _change = true;
    bool _pause = false;
    bool _isOff = false;
};
#endif
