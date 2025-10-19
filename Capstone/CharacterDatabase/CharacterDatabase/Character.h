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


#ifndef CHARACTER_H
#define CHARACTER_H

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <functional>


//==========================================
// Character Data Structure
// All Stats and Abilities for a Character
//==========================================
struct Character {
    std::string name;
    std::string ability1;
    std::string ability2;
    std::string ability3;
    std::string ability4;
    int gunDPS;
    float bulletDMG;
    int ammo;
    float bulletSpeed;
    int lightMeleeDMG;
    int heavyMeleeDMG;
    int health;
    float regen;
    float bulletResist;
    float spiritResist;
    float speed;
    float sprint;
    int stamina;

};


//==================================
// Node Structure
//==================================
struct Node {
    Character data;
    Node* left;
    Node* right;

    //Constructor to initialize node with character
    Node(Character c);

};

//==================================
// Character BST Class
//==================================
class CharacterBST {
private:
    Node* root;

    Node* insert(Node* node, const Character& c);       //Insert Character into Subtree
    Node* search(Node* node, const std::string& name);  //Search subtree for character
    void inorder(Node* node);                           //In order Traversal
    Node* findMin(Node* node);                          //find smallest node
    Node* remove(Node* node, const std::string& name);  //Delete node from subtree
    void destroy(Node* node); 

public:
    // Initializes empty tree
    CharacterBST();
    ~CharacterBST();

    //Interface functions
    void insert(const Character& c);
    Character* search(const std::string& name);
    void displayAll();
    void update(const std::string& name, const Character& updated);
    void remove(const std::string& name);

    Node* getRoot() { return root; }
};

//==================================
// Character Database Class
//==================================
class CharacterDatabase {

private:
    //BST to store the characters
    CharacterBST bst;

        
public:

    //Character functions
    void loadFromDB(const std::string& dbFile);
    void addCharacter(const Character& c);
    void displayCharacters();
    Character* findCharacter(const std::string& name);
    void updateCharacter(const std::string& name, const Character& c);
    void deleteCharacter(const std::string& name);

    // Return all characters in sorted order
    std::vector<Character> getAllCharacters() {
        std::vector<Character> all;
        std::function<void(Node*)> traverse = [&](Node* node) {
            if (!node) return;
            traverse(node->left);
            all.push_back(node->data);
            traverse(node->right);
            };
        traverse(bst.getRoot());
        return all;
    }
};

#endif