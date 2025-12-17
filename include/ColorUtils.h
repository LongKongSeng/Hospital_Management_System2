#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#include <windows.h>
#include <iostream>

using namespace std;

// Simplified color codes - only Blue, Yellow, White, Black
enum ConsoleColor {
    BLACK = 0,
    BLUE = 9,
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

