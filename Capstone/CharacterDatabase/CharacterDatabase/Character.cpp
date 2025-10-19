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
#include <fstream>
#include <sstream>
#include <sqlite3.h>
#include <stdexcept>

//Constructor for a node that wraps a character
Node::Node(Character c) : data(c), left(nullptr), right(nullptr) {}

//Constructor and destructor
CharacterBST::CharacterBST() : root(nullptr) {}
CharacterBST::~CharacterBST() { destroy(root); }

//======================================
//			BST Insert
//======================================
Node* CharacterBST::insert(Node* node, const Character& c) {
	//Empty Spot Create node
	if (!node) return new Node(c);
	//Go left if smaller
	if (c.name < node->data.name) {
		node->left = insert(node->left, c);
	} 
	// Go right if larger
	else if (c.name > node->data.name) {
		node->right = insert(node->right, c);
	}
	else {
		// Catch duplicate names
		throw std::runtime_error("Insert failed: Character with name '" + c.name + "' already exists.");

	}
	return node;
}
//======================================
//			BST Search
//======================================
Node* CharacterBST::search(Node* node, const std::string& name) {
	//Base Case
	if (!node) return nullptr;
	//Found
	if (name == node->data.name) return node;
	// Search Left
	if (name < node->data.name) return search(node->left, name);
	// Search Right
	return search(node->right, name);
}

//======================================
//		 BST InOrder Traversal
//		 Left -> Root -> Right
//======================================
void CharacterBST::inorder(Node* node) {
	if (node) {
		inorder(node->left);
		std::cout << "Character: " << node->data.name
			<< " | Gun DPS: " << node->data.gunDPS
			<< " | Health: " << node->data.health << std::endl;

		inorder(node->right);
	}
}

// Helper to find leftmost node
Node* CharacterBST::findMin(Node* node) {
	while (node && node->left) node = node->left;
	return node;
}

// Recursive remover
Node* CharacterBST::remove(Node* node, const std::string& name) {
	if (!node) return nullptr;

	if (name < node->data.name) {
		node->left = remove(node->left, name);
	}
	else if (name > node->data.name) {
		node->right = remove(node->right, name);
	}
	else {
		// Node found: 3 cases
		if (!node->left && !node->right) {
			delete node;
			return nullptr;
		}
		else if (!node->left) {
			Node* temp = node->right;
			delete node;
			return temp;
		}
		else if (!node->right) {
			Node* temp = node->left;
			delete node;
			return temp;
		}
		else {
			// Two children: replace with inorder successor (min in right subtree)
			Node* minRight = findMin(node->right);
			node->data = minRight->data;
			node->right = remove(node->right, minRight->data.name);
		}
	}
	return node;
}

//Recursive destructor helper
void CharacterBST::destroy(Node* node) {
	if (node) {
		destroy(node->left);
		destroy(node->right);
		delete node;
	}
}



//======================================
//		 Public BST Functions
//======================================
void CharacterBST::insert(const Character& c) { root = insert(root, c); }

// BST Search
Character* CharacterBST::search(const std::string& name) {
	Node* result = search(root, name);
	return result ? &result->data : nullptr;
}

//BST Display all
void CharacterBST::displayAll() {
	if (!root) {
		std::cout << "Database is empty." << std::endl;
		return;
	}
	inorder(root);
}

//BST Update
void CharacterBST::update(const std::string& name, const Character& updated) {
	Node* node = search(root, name);
	if (!node) {
		throw std::runtime_error("Update failed: Character '" + name + "' not found.");
	}
	node->data = updated;
	std::cout << "updated: " + name;
}
//BST Remove
void CharacterBST::remove(const std::string& name) {
	if (!search(root, name)) {
		throw std::runtime_error("Delete failed: Character '" + name + "' not found.");
	}
	root = remove(root, name);
	std::cout << "removed: " + name;
}

//======================================
//	CharacterDatabase Implementation
//======================================

// Loads characters from SQLite3 Database
void CharacterDatabase::loadFromDB(const std::string& dbFile) {
	sqlite3* db;
	sqlite3_stmt* stmt;

	//Open Database
	int rc = sqlite3_open(dbFile.c_str(), &db);
	if (rc != SQLITE_OK) {
		throw std::runtime_error("Cannot Open Database: " + std::string(sqlite3_errmsg(db)));
	}

	// If table does not exist create it
	const char* createTableSQL =
		"CREATE TABLE IF NOT EXISTS Characters ("
		"Name TEXT PRIMARY KEY, "
		"Ability1 TEXT, Ability2 TEXT, Ability3 TEXT, Ability4 TEXT, "
		"DPS REAL, BulletDMG REAL, Ammo INTEGER, BulletPS REAL, "
		"LightMelee INTEGER, HeavyMelee INTEGER, "
		"MaxHealth INTEGER, HealthRegen REAL, BulletResist REAL, SpiritResist REAL, "
		"MoveSpeed REAL, SprintSpeed REAL, Stamina INTEGER);";

	rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, nullptr);
	if (rc != SQLITE_OK) throw std::runtime_error("Failed to create table: " + std::string(sqlite3_errmsg(db)));

	// Prepare Select Query
	const char* sql = "SELECT Name, Ability1, Ability2, Ability3, Ability4, "
		"DPS, BulletDMG, Ammo, BulletPS, LightMelee, HeavyMelee, "
		"MaxHealth, HealthRegen, BulletResist, SpiritResist, MoveSpeed, SprintSpeed, Stamina "
		" FROM Characters;";

	rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
	if (rc != SQLITE_OK) {
		std::string err = sqlite3_errmsg(db);
		sqlite3_close(db);
		throw std::runtime_error("Failed to prepare statement: " + err);
	}

	//Loop through results and insert into BST
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		Character c;
		c.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
		c.ability1 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
		c.ability2 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
		c.ability3 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
		c.ability4 = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
		c.gunDPS = static_cast<int>(sqlite3_column_double(stmt, 5));
		c.bulletDMG = static_cast<float>(sqlite3_column_double(stmt, 6));
		c.ammo = static_cast<int>(sqlite3_column_double(stmt, 7));
		c.bulletSpeed = static_cast<float>(sqlite3_column_double(stmt, 8));
		c.lightMeleeDMG = static_cast<int>(sqlite3_column_double(stmt, 9));
		c.heavyMeleeDMG = static_cast<int>(sqlite3_column_double(stmt, 10));
		c.health = static_cast<int>(sqlite3_column_double(stmt, 11));
		c.regen = static_cast<float>(sqlite3_column_double(stmt, 12));
		c.bulletResist = static_cast<float>(sqlite3_column_double(stmt, 13));
		c.spiritResist = static_cast<float>(sqlite3_column_double(stmt, 14));
		c.speed = static_cast<float>(sqlite3_column_double(stmt, 15));
		c.sprint = static_cast<float>(sqlite3_column_double(stmt, 16));
		c.stamina = static_cast<int>(sqlite3_column_double(stmt, 17));

		try {
			bst.insert(c);
		}
		catch (std::exception& e) {
			std::cerr << "Warning: " << e.what() << " Skipping duplicate in BST.\n";
		}
	}

	//Check for errors
	if (rc != SQLITE_DONE) {
		std::string err = sqlite3_errmsg(db);
		sqlite3_finalize(stmt);
		sqlite3_close(db);
		throw std::runtime_error("Error Reading Database: " + err);
	}

	sqlite3_finalize(stmt);
	sqlite3_close(db);
}

//===================================
// Crud Wrapper Functions for DB
//===================================
void CharacterDatabase::addCharacter(const Character& c) { bst.insert(c); }
void CharacterDatabase::displayCharacters() { bst.displayAll(); }
Character* CharacterDatabase::findCharacter(const std::string& name) { return bst.search(name); }
void CharacterDatabase::updateCharacter(const std::string& name, const Character& c) { bst.update(name, c); }
void CharacterDatabase::deleteCharacter(const std::string& name) { bst.remove(name); }