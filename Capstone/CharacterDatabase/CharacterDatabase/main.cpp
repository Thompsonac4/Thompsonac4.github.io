// ===========================================================
// Capstone Project
// CRUD Functionality - BST - SQLite - Html Report
// Author: Austin Thompson
// ------------------------------------------------------
// Version: 1.4 | Date: 2025-10-19
// Description: This code provides a basic structure for 
// CRUD operations without a database connection.
// Includes a Binary Search Tree for searching, imports
// from a SQLite Database, and creates a html Report
//-------------------------------------------------------
// ===========================================================

#include "Character.h"
#include <iostream>
#include <sqlite3.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

//===========================================================
// exportToHTML - Generates an HTML Report from updated DB
//===========================================================
void exportToHTML(CharacterDatabase& db, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Cannot open HTML file for writing");

    //Retrieve all characters from the database
    auto characters = db.getAllCharacters();
    //Exit if it is empty
    if (characters.empty()) return;

    // Create Header and set Styling
    file << "<!DOCTYPE html>\n<html>\n<head>\n";
    file << "<meta charset='UTF-8'>\n";
    file << "<title>Character Report</title>\n";
    file << "<style>\n";
    file << "body { background-color: #62544a; color: #c09f7e; font-family: Arial, sans-serif; }\n";
    file << "table { border-collapse: collapse; width: 100%; }\n";
    file << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
    file << "th { background-color: #62544a; }\n";
    file << ".chart-bar { fill: #93a0fe; }\n";
    file << ".chart-label { font-size: 10px; text-anchor: middle; }\n";
    file << ".chart-value { font-size: 10px; text-anchor: middle; fill:#62544a; }\n";
    file << "</style>\n</head>\n<body>\n";

    // Title
    file << "<h1>Character Database Report</h1>\n";

    // Table of characters
    file << "<h2>Character Stats</h2>\n";
    file << "<table>\n<tr><th>Name</th><th>Gun DPS</th><th>Health</th></tr>\n";
    for (auto& c : characters) {
        file << "<tr><td>" << c.name << "</td><td>" << c.gunDPS
            << "</td><td>" << c.health << "</td></tr>\n";
    }
    file << "</table>\n";

    // SVG Bar chart based on Gun DPS
    file << "<h2>Gun DPS Chart</h2>\n";
    int svgWidth = 1400;
    int svgHeight = 300;
    file << "<svg width='" << svgWidth << "' height='" << svgHeight << "'>\n";

    // Determine max DPS for scaling
    int maxDPS = 0;
    for (auto& c : characters) if (c.gunDPS > maxDPS) maxDPS = c.gunDPS;

    size_t n = characters.size();
    int spacing = 20;
    int barWidth = (svgWidth - spacing * (int)n) / (int)n;

    // Loop for bar and labels
    for (size_t i = 0; i < n; ++i) {
        float barHeight = (characters[i].gunDPS / (float)maxDPS) * 200;
        int x = i * (barWidth + spacing);
        int y = 250 - barHeight;

        // Draw bar
        file << "<rect x='" << x << "' y='" << y
            << "' width='" << barWidth << "' height='" << barHeight
            << "' class='chart-bar'></rect>\n";

        // Write DPS Number on top
        file << "<text x='" << (x + barWidth / 2) << "' y='" << (y - 5)
            << "' class='chart-value'>" << characters[i].gunDPS << "</text>\n";

        // Write Name below
        file << "<text x='" << (x + barWidth / 2) << "' y='265' class='chart-label'>"
            << characters[i].name << "</text>\n";
    }

    file << "</svg>\n";
    file << "</body>\n</html>";
    file.close();

    std::cout << "HTML report generated: " << filename << std::endl;
}

//===========================================================
// Main - Showcases basic CRUD functionality
//===========================================================
int main() {
    try {
        // Create database instance
        CharacterDatabase db;
        // Loads from csv
        db.loadFromDB("characters.db");

        // Displays characters
        std::cout << "=== All Characters ===" << std::endl;
        db.displayCharacters();

        // Searches based on character name, prints info if found.
        std::cout << "\n=== Search Result ===" << std::endl;
        Character* found = db.findCharacter("Pocket");
        if (found) {
            std::cout << "Found " << found->name
                << " | Gun DPS: " << found->gunDPS
                << " | Health: " << found->health << std::endl;
        }
        else {
            std::cout << "Character not found!" << std::endl;
        }

        // Searches for character and updates its health value
        std::cout << "\n=== Update Pocket ===" << std::endl;
        if (found) {
            Character updated = *found;
            updated.health = 2000;
            db.updateCharacter("Pocket", updated);
        }
        
        // Deletes a character from list
        std::cout << "\n=== Delete Yamato ===" << std::endl;
        db.deleteCharacter("Yamato");
        //Displays in command line characters after adjustments
        std::cout << "\n=== Characters After Deletion ===" << std::endl;
        db.displayCharacters();
        //Exports to html
        std::cout << "\n=== Exported to HTML ===" << std::endl;
        exportToHTML(db, "CharacterReport.html");
    }
    catch (std::exception& e) {
        // Catches and displays unhandled errors
        std::cerr << "Fatal Error: " << e.what() << std::endl;
    }
    return 0;
}

