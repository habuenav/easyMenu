#pragma once

#define U8g2 0 
#define GFX 1
#define event(btn_expr) []() { return btn_expr; }
#define NUMITEMS(arg) ((unsigned int) (sizeof (arg) / sizeof (arg [0])))
#include "Arduino.h"

#if LIBRARY == U8g2
    #include <U8g2lib.h>
#elif LIBRARY == GFX
    #include <Adafruit_GFX.h>
#endif

typedef String txtMenu;
typedef bool (*MenuEventCallback)();

// Modos de resaltado
enum HighlightMode { HIGHLIGHT_BOX, HIGHLIGHT_CURSOR };

inline bool isNumeric(const String& str) {
    if (str.length() == 0) return false;
    for (char c : str) if (!isdigit(c)) return false;
    return true;
}

template<typename T>
uint8_t size(T (&arr)) { 
    uint8_t n = NUMITEMS(arr);
    n = (n > 0) ? n - 1 : n;  
    if(arr[n][arr[n].length() - 1] != ' ' && n > 0) {       arr[n] = String(arr[n] + ' ');  }
    else {  n = 0; do { n++; } while(arr[n][arr[n].length() - 1] != ' ');  }
    return n + 1;    
}
inline const char *getStringLineStart(uint8_t line_idx, const char *str) {
    uint8_t line_cnt = 1;
    if (line_idx == 0) return str;
    while (*str != '\0') { if (*str++ == '|') { if (line_cnt == line_idx) return str; line_cnt++; } }
    return NULL;
}
inline uint8_t stringLineCnt(const char *str) {
    uint8_t line_cnt = 1;
    if (str == NULL) return 0;
    while (*str != '\0') { if (*str++ == '|') line_cnt++; }
    return line_cnt;
}
inline void copyStringLine(char *dest, uint8_t line_idx, const char *str) {
    if (dest == NULL) return;
    const char *p = getStringLineStart(line_idx, str);
    if (p != NULL) { while (*p != '|' && *p != '\0') { *dest++ = *p++; } }
    *dest = '\0';
}
inline void destroyStrings(txtMenu* arr, uint8_t count) {
    if (arr == nullptr) return;
    for (uint8_t i = 0; i < count; i++) arr[i].~String();
    free(arr);
}

enum { LEFT, CENTER, RIGHT };

class easyMenu {
public:
    easyMenu& attachUp(MenuEventCallback cb) { return _upCallback = cb, *this; }
    easyMenu& attachDown(MenuEventCallback cb) { return _downCallback = cb, *this; }
    easyMenu& attachEnter(MenuEventCallback cb) { return _enterCallback = cb, *this; }
    easyMenu& attachRoot(MenuEventCallback cb) { return _rootCallback = cb, *this; }

#if LIBRARY == U8g2
    easyMenu(U8G2& dispPointer) : disp(dispPointer) {
        uint16_t area = getLcdWidth() * getLcdHeight();
        if (!titleFont && !itemFont) {
            if (area <= 2880) { titleFont = u8g2_font_wedge_tr; itemFont = u8g2_font_tiny5_tf; }
            else if (area <= 4096 && getLcdHeight() <= 32) { titleFont = u8g2_font_squeezed_b6_tr; itemFont = u8g2_font_spleen5x8_mf; }
            else { titleFont = u8g2_font_roentgen_nbp_tr; itemFont = u8g2_font_resoledbold_tr; }
        }
        titleFont = titleFont ? titleFont : itemFont;
        itemFont = itemFont ? itemFont : titleFont;
    }
    const uint8_t *titleFont = NULL;
    const uint8_t *itemFont = NULL;

#elif LIBRARY == GFX
    easyMenu(Adafruit_GFX& dispPointer) : disp(dispPointer) {
        uint16_t area = getLcdWidth() * getLcdHeight();
        if (area <= 2880) { _tx = 1.2; _ty = 1.2; _ix = 1.0; _iy = 1.0; }
        else if (area <= 3100) { _tx = 1.5; _ty = 1.4; _ix = 1.1; _iy = 1.1; }
        else    { _tx = 1.7; _ty = 1.6; _ix = 1.6; _iy = 1.5; }
    }
#endif

    ~easyMenu() { destroyStrings(_opt, _optCount); destroyStrings(_root_opt, _rootCount); }

    void begin() { dispInit();  dispClear();  calculateLayout();  _lastActivity = millis();  _isOff = false;  }

    void addOption(String nuevaOpcion) {
        if (format && nuevaOpcion.length() > 0) { nuevaOpcion.toLowerCase(); nuevaOpcion[0] = toUpperCase(nuevaOpcion[0]); }

        uint8_t nuevoTamano = _nroItems + 2; 
        txtMenu* ptr_temporal = (txtMenu*) realloc(_opt, nuevoTamano * sizeof(txtMenu));

        if (ptr_temporal != nullptr) {
            _opt = ptr_temporal;
            new (&_opt[nuevoTamano - 1]) String(nuevaOpcion);
            _nroItems = nuevoTamano - 1; 
            _optCount = nuevoTamano;
            _change = true;      
            calculateLayout();   
            checkSelectedScroll();
        }
    }

   void show() {
        if (_messageUntil > 0) {
            if (millis() < _messageUntil) return;
            _messageUntil = 0;
            _change = true;
        }
        if (_upCallback && _upCallback()) up();
        if (_downCallback && _downCallback()) down();
        if (_rootCallback && _rootCallback()) root();

        checkAutoOff();
        
        if (!_isOff && !_pause && _selectedNeedsScroll) {
            uint32_t scrollDelay = _scrollReset ? 1500 : 350;
            if (millis() - _lastScrollTime > scrollDelay) {
                _scrollReset = false;
                _scrollPos++;
                _lastScrollTime = millis();
                _change = true;
                if (_scrollPos >= _opt[_selected].length()) { _scrollPos = 0; _scrollReset = true; }
            }
        }

        if (_isOff || !(_change && !_pause)) return;

        dispClear();
        if (_title) { setFontTitle(); dispPrint(_opt[0].c_str(), alignX(alignTitle, _opt[0].c_str()), 0, false); }
        setFontItem();

        // Determinar índice inicial según el scroll vertical
        uint8_t firstItemIdx = _title ? 1 : 0;
        uint8_t initLine = (_selected < (firstItemIdx + _max_lines)) ? firstItemIdx : (_selected - _max_lines + 1);
        uint8_t maxRenderLine = min((int)_nroItems, (int)(initLine + _max_lines - 1));
        uint16_t startY = _title ? (_titleht + _interline) : 0;

        for (uint8_t i = initLine; i <= maxRenderLine; i++) {
            uint8_t slotIndex = i - initLine; 
            String textoSub;
            const char* textoPtr = (i == _selected && _scrollPos > 0) ? (textoSub = _opt[i].substring(_scrollPos), textoSub.c_str()) : _opt[i].c_str();
            uint16_t cur_y = startY + slotIndex * (_lineht + _interline);
            // Verificación estricta: si el texto excede la pantalla, cancelamos la impresión de este ítem
            if ((cur_y + _lineht) > getLcdHeight()) break;
            if (i == _selected && _highlightMode == HIGHLIGHT_CURSOR) {
                String fullText = _cursorSymbol + String(" ") + String(textoPtr);
                uint8_t cur_x = alignX(alignItem, fullText.c_str());
                dispPrint(fullText.c_str(), cur_x, cur_y, false);
            } 
            else {  uint8_t cur_x = alignX(alignItem, textoPtr);  dispPrint(textoPtr, cur_x, cur_y, (i == _selected));  }
        }
        dispFlush();
        _change = false;
    }

    easyMenu& setAutoOff(uint16_t timeout) { return _timeout = timeout * 1000, _lastActivity = millis(), _isOff = false, *this; }
    easyMenu& setInterline(int8_t val) { return interline = val, _change = true, calculateLayout(), *this; }
    easyMenu& setMaxLines(int8_t val) { return max_lines = val, _change = true, calculateLayout(), *this; }

    // Configuración del cursor/resaltado
    easyMenu& setCursor(const String& symbol) {
        if (symbol == "" || symbol == "BOX") { _highlightMode = HIGHLIGHT_BOX;   _cursorSymbol = ""; }
        else { _highlightMode = HIGHLIGHT_CURSOR;  _cursorSymbol = symbol;  }
        _change = true;
        checkSelectedScroll();
        return *this;
    }

    bool enter() { return (_enterCallback && _enterCallback()) ? (_lastActivity = millis(), _isOff = false, true) : false; }    
    void up() { handleNav((_selected <= _title) ? _nroItems : _selected - 1); }
    void down() { handleNav((_selected < _nroItems) ? _selected + 1 : _title); }
    void fullUp() { handleNav(_title ? 1 : 0); }
    void fullDown() { handleNav(_nroItems); }

    void message(const String& _text, uint8_t align = LEFT, int wait = 2000) {
        _lastActivity = millis();
        _isOff = false;
        const char* text = _text.c_str();
        uint8_t total_lines = stringLineCnt(text);
        dispClear();
        setFontItem();
        uint16_t font_height = getFontHeight();
        char buf[_text.length() + 1];
        uint16_t total_height = total_lines * font_height;

        for (uint8_t i = 0; i < total_lines; i++) {
            copyStringLine(buf, i, text);
            uint8_t cur_x = alignX(align, buf);
            uint16_t cur_y = ((getLcdHeight() - total_height) / 2) + (i * font_height);
            dispPrint(buf, cur_x, cur_y, false);
        }
        dispFlush();
        _messageUntil = millis() + wait;
    }

    template <typename T, class tipo>
    void assign(T &opt, tipo id, bool title = true) {
        allocateAndFormat(_opt, _optCount, opt, id, title);
        _nroItems = size(opt) - 1;
        _title = title;
        _selected = title ? 1 : 0;
        calculateLayout();
        checkSelectedScroll();
    }

    template <typename T, class tipo>
    void root(T &opt, tipo id, bool title = true) {
        allocateAndFormat(_root_opt, _rootCount, opt, id, title, true);
        _title = title;
        assign(_root_opt, id, title);
    }

    void root() {
        if (_root_id != -1) assign(_root_opt, _root_id, _title);
        else if (_root_Id != "") assign(_root_opt, _root_Id, _title);
    }

    easyMenu& setTitleSize(float w, float h) { 
        #if LIBRARY == GFX
        _tx = w; _ty = h; _change = true; calculateLayout();
        #endif
        return *this; 
    }

    easyMenu& setItemSize(float w, float h) { 
        #if LIBRARY == GFX
        _ix = w; _iy = h; _change = true; calculateLayout();
        #endif
        return *this; 
    }

    template <class tipo>
    bool select(uint8_t sel, tipo id) {
        _lastActivity = millis();
        _isOff = false;
        String test = String(id);
        return isNumeric(test) ? (test.toInt() == _id && _selected == sel) : (test == _Id && _selected == sel);
    }

    void pause(bool opt) { _pause = opt; }
    int8_t getInterline() { return _interline; }

    bool format = true;
    uint8_t alignTitle = LEFT;
    uint8_t alignItem = LEFT;
    int8_t interline = -1;
    int8_t max_lines = -1;
    uint8_t _selected = 0;

private:
    MenuEventCallback _upCallback = nullptr;
    MenuEventCallback _downCallback = nullptr;
    MenuEventCallback _enterCallback = nullptr;
    MenuEventCallback _rootCallback = nullptr;

    uint32_t _lastScrollTime = 0;
    uint8_t _scrollPos = 0;
    bool _scrollReset = true;
    bool _selectedNeedsScroll = false;
    uint32_t _messageUntil = 0;

    HighlightMode _highlightMode = HIGHLIGHT_BOX;
    String _cursorSymbol = "";

    float _tx = 1.7;
    float _ty = 1.6;
    float _ix = 1.6;
    float _iy = 1.5;

//===========================================================================
#if LIBRARY == U8g2
    U8G2& disp;
    inline void dispInit() { disp.begin(); }
    inline void dispClear() { disp.clearBuffer(); }
    inline void dispFlush() { disp.sendBuffer(); }
    inline uint16_t getLcdWidth() { return disp.getDisplayWidth(); }
    inline uint16_t getLcdHeight() { return disp.getDisplayHeight(); }
    inline void setFontTitle() { disp.setFont(titleFont); disp.setFontPosTop(); }
    inline void setFontItem() { disp.setFont(itemFont); disp.setFontPosTop(); }
    inline uint16_t getFontHeight() { return disp.getMaxCharHeight() + 1; }
    inline uint16_t getTextWidth(const char* text) { return disp.getStrWidth(text); }
    inline void dispPrint(const char* text, uint16_t x, uint16_t y, bool isSelected) {
        disp.setCursor(x, y);
        if (isSelected && _highlightMode == HIGHLIGHT_BOX) {
            disp.drawBox(2, y, getLcdWidth() - 4, _lineht - 1);
            disp.setDrawColor(0);
            disp.print(text);
            disp.setDrawColor(1);
        } 
        else {  disp.print(text);  }
    }
#elif LIBRARY == GFX
    Adafruit_GFX& disp;
    #define updateMenu display.display(); 
    inline void dispInit() {}   
    inline void dispClear() { disp.fillScreen(0); }
    inline void dispFlush() {}  
    inline uint16_t getLcdWidth() { return disp.width(); } 
    inline uint16_t getLcdHeight() { return disp.height(); }
    inline void setFontTitle() { disp.setTextSize(_tx, _ty); disp.setTextColor(1); }
    inline void setFontItem() { disp.setTextSize(_ix, _iy); }
    inline uint16_t getFontHeight() { int16_t x1, y1; uint16_t w, h; disp.getTextBounds("A", 0, 0, &x1, &y1, &w, &h); return h+1; }
    inline uint16_t getTextWidth(const char* text) { int16_t x1, y1; uint16_t w, h; disp.getTextBounds(text, 0, 0, &x1, &y1, &w, &h); return w; }
    inline void dispPrint(const char* text, uint16_t x, uint16_t y, bool isSelected) {
        disp.setCursor(x, y);
        if (isSelected && _highlightMode == HIGHLIGHT_BOX) {
            disp.setTextColor(0, 1);
        } else {
            disp.setTextColor(1, 0);
        }
        disp.print(text);
    }
#endif

    inline uint8_t alignX(uint8_t align, const char* text) {
        return (align == CENTER) ? (getLcdWidth() - getTextWidth(text)) / 2 :
               (align == RIGHT)  ? (getLcdWidth() - getTextWidth(text)) : 2;
    }
// =========================================================================

    void checkSelectedScroll() {
        if (_opt == nullptr || _selected > _nroItems) { _selectedNeedsScroll = false;  return;   }
        uint16_t textW = getTextWidth(_opt[_selected].c_str());
        if (_highlightMode == HIGHLIGHT_CURSOR) {  textW += getTextWidth((_cursorSymbol + " ").c_str());  }
        _selectedNeedsScroll = (textW > (getLcdWidth() - 6));
    }

    void calculateLayout() {
        setFontTitle();
        _titleht = _title ? getFontHeight() : 0;
        setFontItem();
        _lineht = getFontHeight();
        int16_t availableHeight = max(0, (int)(getLcdHeight() - _titleht));
        uint8_t totalItems = (_nroItems >= (uint8_t)_title) ? (_nroItems + 1 - _title) : 0;
        uint8_t max_fit_lines = availableHeight / _lineht;
        _max_lines = (max_lines != -1)? min((uint8_t)max_lines, max_fit_lines):_max_lines = max_fit_lines;

        if (interline != -1) {  _interline = interline;  }
        else {
            uint8_t visible_lines = min(totalItems, _max_lines);
            if (visible_lines > 0) {
                int16_t remainingPixels = availableHeight - (visible_lines * _lineht);
                uint8_t gaps = visible_lines + (_title ? 1 : 0) - 1;
                _interline = (gaps > 0) ? (remainingPixels / gaps) : 0;
                _interline = max((int8_t)0, min(_interline, (int8_t)_lineht));
            }
            else {  _interline = 0; }
        }

        while (_max_lines > 0) {
            uint8_t visible_lines = min(totalItems, _max_lines);
            if (visible_lines == 0) break;
            uint16_t required_height = visible_lines * _lineht;
            if (visible_lines > 1 || _title) {
                uint8_t gaps = visible_lines - 1 + (_title ? 1 : 0);
                required_height += gaps * _interline;
            }
            if (required_height <= availableHeight) break; // Cabe perfectamente sin cortar nada
            _max_lines--; // Reducir líneas si la combinación excede la pantalla
        }
    }

    void handleNav(uint8_t newSel) { 
        _change = true, _lastActivity = millis(), _isOff = false, _selected = newSel; 
        _scrollPos = 0, _scrollReset = true, _lastScrollTime = millis();
        checkSelectedScroll();
    }

    void checkAutoOff() {
        if (_timeout > 0 && !_isOff && (millis() - _lastActivity > _timeout)) { dispClear();  dispFlush();  _isOff = true;  }
    }

    template <typename T, class tipo>
    void allocateAndFormat(txtMenu* &dest, uint8_t &destCount, T (&opt), tipo id, bool title, bool isRoot = false) {
        destroyStrings(dest, destCount);
        uint8_t tam = size(opt);
        String test = String(id);
        
        if (isNumeric(test)) (isRoot ? _root_id : _id) = test.toInt();
        else (isRoot ? _root_Id : _Id) = test;
        
        dest = (txtMenu*) malloc(tam * sizeof(txtMenu));
        for (uint8_t i = 0; i < tam; i++) {
            new (&dest[i]) String(opt[i]); 
            if (format) {
                dest[i].toLowerCase();
                if (dest[i].length() > 0) dest[i][0] = toUpperCase(dest[i][0]);
                if (title && i == 0) dest[0].toUpperCase();
            }
        }
        destCount = tam;
        _change = true, _isOff = false, _lastActivity = millis();
    }

    txtMenu *_opt = nullptr;
    uint8_t _optCount = 0;
    txtMenu *_root_opt = nullptr;
    uint8_t _rootCount = 0;
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
