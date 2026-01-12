#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#include <windows.h>
#include <iostream>

using namespace std;

// Color codes - Blue shades, Yellow, White, Black
enum ConsoleColor {
    BLACK = 0,
    DARK_BLUE = 1,
    DARK_GREEN = 2,
    DARK_CYAN = 3,
    DARK_RED = 4,
    DARK_MAGENTA = 5,
    DARK_YELLOW = 6,
    GRAY = 7,
    DARK_GRAY = 8,
    BLUE = 9,
    LIGHT_BLUE = 9,
    GREEN = 10,
    LIGHT_GREEN = 10,     // Added for charts
    CYAN = 11,
    LIGHT_CYAN = 11,
    RED = 12,
    LIGHT_RED = 12,       // Added for errors
    MAGENTA = 13,
    LIGHT_MAGENTA = 13,   // Added for yearly charts
    YELLOW = 14,
    WHITE = 15
};

class ColorUtils {
public:
    static void setColor(int foreground, int background = BLACK) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, foreground | (background << 4));
    }
    
    static void resetColor() {
        setColor(WHITE, BLACK);
    }
    
    // Colorful text functions
    static void printColored(const string& text, int color) {
        setColor(color);
        cout << text;
        resetColor();
    }
    
    // Print with background color
    static void printColoredBG(const string& text, int foreground, int background) {
        setColor(foreground, background);
        cout << text;
        resetColor();
    }
    
    // Print title with special formatting
    static void printTitle(const string& text, int color) {
        setColor(color);
        cout << text;
        resetColor();
    }
    
    // Print menu option with color
    static void printMenuOption(const string& text, int color) {
        setColor(color);
        cout << text;
        resetColor();
    }
};

#endif

