#ifndef MENU_NAVIGATOR_H
#define MENU_NAVIGATOR_H

#include <windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <conio.h>
#include "ColorUtils.h"

using namespace std;

class MenuNavigator {
public:
    // Display menu with arrow key navigation
    // Returns the selected index (0-based)
    static int showMenu(const vector<string>& options, const string& title = "", bool showExitOption = true) {
        int selected = 0;
        int totalOptions = options.size();
        
        while (true) {
            system("cls");
            
            // Display title if provided
            if (!title.empty()) {
                displayTitle(title);
            }
            
            // Display menu options
            ColorUtils::setColor(WHITE);
            cout << "\n+----------------------------------------+" << endl;
            ColorUtils::resetColor();
            
            for (int i = 0; i < totalOptions; i++) {
                ColorUtils::setColor(WHITE);
                cout << "|  ";
                
                if (i == selected) {
                    // Highlighted option: Yellow text on Blue background
                    ColorUtils::printColoredBG(options[i], YELLOW, BLUE);
                } else {
                    // Normal option: White text
                    ColorUtils::setColor(WHITE);
                    cout << options[i];
                }
                
                // Fill remaining space
                int spaces = 38 - options[i].length();
                for (int j = 0; j < spaces; j++) cout << " ";
                
                ColorUtils::setColor(WHITE);
                cout << " |" << endl;
            }
            
            ColorUtils::setColor(WHITE);
            cout << "+----------------------------------------+" << endl;
            ColorUtils::resetColor();
            
            // Instructions
            ColorUtils::setColor(WHITE);
            cout << "\nUse UP/DOWN arrow keys to navigate, ENTER to select" << endl;
            if (showExitOption) {
                cout << "Press ESC to go back" << endl;
            }
            ColorUtils::resetColor();
            
            // Get keyboard input
            int key = _getch();
            
            // Handle arrow keys and special keys
            if (key == 224) { // Arrow key prefix
                key = _getch();
                switch (key) {
                    case 72: // UP arrow
                        selected = (selected - 1 + totalOptions) % totalOptions;
                        break;
                    case 80: // DOWN arrow
                        selected = (selected + 1) % totalOptions;
                        break;
                }
            } else if (key == 13) { // ENTER
                return selected;
            } else if (key == 27 && showExitOption) { // ESC
                return -1; // Exit/Back
            }
        }
    }
    
    // Display title with Blue border and Yellow text on Blue background
    static void displayTitle(const string& title) {
        ColorUtils::setColor(BLUE);
        cout << "\n+----------------------------------------------------------------------+" << endl;
        cout << "|                                                                |" << endl;
        ColorUtils::resetColor();
        
        // Calculate padding for centered title
        int padding = (62 - title.length()) / 2;
        
        ColorUtils::setColor(WHITE);
        cout << "|";
        for (int i = 0; i < padding; i++) cout << " ";
        ColorUtils::printColoredBG(title, YELLOW, BLUE);
        for (int i = 0; i < (62 - title.length() - padding); i++) cout << " ";
        cout << "|" << endl;
        
        ColorUtils::setColor(BLUE);
        cout << "|                                                                |" << endl;
        cout << "+----------------------------------------------------------------------+" << endl;
        ColorUtils::resetColor();
    }
    
    // Get arrow key input (returns: 0=UP, 1=DOWN, 2=ENTER, 3=ESC, -1=other)
    static int getArrowKey() {
        int key = _getch();
        if (key == 224) {
            key = _getch();
            if (key == 72) return 0; // UP
            if (key == 80) return 1; // DOWN
        } else if (key == 13) return 2; // ENTER
        else if (key == 27) return 3; // ESC
        return -1;
    }
};

#endif

