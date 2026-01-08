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
    // Display menu with arrow key navigation matching the GUI style
    // Returns the selected index (0-based)
    static int showMenu(const vector<string>& options, const string& title = "", bool showExitOption = true) {
        int selected = 0;
        int totalOptions = static_cast<int>(options.size());
        const int SEPARATOR_LENGTH = 80; // Length of separator line
        
        while (true) {
            system("cls");
            
            // Top separator line
            ColorUtils::setColor(LIGHT_BLUE);
            for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
            ColorUtils::resetColor();
            cout << endl;
            
            // Display title if provided (centered, in blue box)
            if (!title.empty()) {
                displayTitle(title, SEPARATOR_LENGTH);
            }
            
            // Separator after title
            ColorUtils::setColor(LIGHT_BLUE);
            for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
            ColorUtils::resetColor();
            cout << endl << endl;
            
            // Display menu options (centered)
            for (int i = 0; i < totalOptions; i++) {
                // Calculate centering for option text
                int optionLength = static_cast<int>(options[i].length());
                if (i == selected) {
                    // Selected option: add > and < markers, use light cyan
                    optionLength += 4; // > < and spaces
                }
                int padding = (SEPARATOR_LENGTH - optionLength) / 2;
                
                // Print padding spaces
                for (int j = 0; j < padding; j++) cout << " ";
                
                if (i == selected) {
                    // Highlighted option: Light cyan text with > and < markers
                    ColorUtils::setColor(LIGHT_CYAN);
                    cout << "> " << options[i] << " <";
                    ColorUtils::resetColor();
                } else {
                    // Normal option: White text
                    ColorUtils::setColor(WHITE);
                    cout << options[i];
                    ColorUtils::resetColor();
                }
                
                // Fill remaining space (if any)
                int remaining = SEPARATOR_LENGTH - padding - static_cast<int>(optionLength);
                for (int j = 0; j < remaining; j++) cout << " ";
                cout << endl;
            }
            
            cout << endl;
            
            // Bottom separator
            ColorUtils::setColor(LIGHT_BLUE);
            for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
            ColorUtils::resetColor();
            cout << endl;
            
            // Instructions (in blue highlighted box)
            string instruction1 = "Use 'UP'/'DOWN' key to move selection and press 'ENTER' to select";
            int inst1Padding = (SEPARATOR_LENGTH - static_cast<int>(instruction1.length())) / 2;
            for (int i = 0; i < inst1Padding; i++) cout << " ";
            ColorUtils::printColoredBG(instruction1, WHITE, BLUE);
            int inst1Remaining = SEPARATOR_LENGTH - inst1Padding - static_cast<int>(instruction1.length());
            for (int i = 0; i < inst1Remaining; i++) cout << " ";
            cout << endl;
            
            if (showExitOption) {
                string instruction2 = "Press 'ESC' key to go back.";
                int inst2Padding = (SEPARATOR_LENGTH - static_cast<int>(instruction2.length())) / 2;
                for (int i = 0; i < inst2Padding; i++) cout << " ";
                ColorUtils::printColoredBG(instruction2, WHITE, BLUE);
                int inst2Remaining = SEPARATOR_LENGTH - inst2Padding - static_cast<int>(instruction2.length());
                for (int i = 0; i < inst2Remaining; i++) cout << " ";
                cout << endl;
            }
            
            // Bottom separator
            ColorUtils::setColor(LIGHT_BLUE);
            for (int i = 0; i < SEPARATOR_LENGTH; i++) cout << "=";
            ColorUtils::resetColor();
            cout << endl;
            
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
    
    // Display title centered in blue highlighted box
    static void displayTitle(const string& title, int separatorLength = 80) {
        // Calculate padding for centered title
        int padding = (separatorLength - static_cast<int>(title.length())) / 2;
        
        // Print spaces before title
        for (int i = 0; i < padding; i++) cout << " ";
        
        // Print title with white text on blue background
        ColorUtils::printColoredBG(title, WHITE, BLUE);
        
        // Fill remaining space after title
        int remaining = separatorLength - padding - static_cast<int>(title.length());
        for (int i = 0; i < remaining; i++) cout << " ";
        cout << endl;
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








