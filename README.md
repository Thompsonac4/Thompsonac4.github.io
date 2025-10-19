# Austin Thompson Capstone Project

<h1 align="center">Capstone - Character Database</h1>

```
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
```
## Enhancement Design and Goals

## Artifact - [Software Design and Engineering](Artifacts/crud.py)
  The enhancement is a C++ program that implements the CRUD functionality for database management and changed to handle character and their stats. Each character has their own stat sheets with different information for weapons and abilities. This project is based on the original crud artifact from CS 340 Python CRUD program that connected to a MongoDB database for an animal shelter. The enhancement to C++ was completed to demonstrate advanced software engineering, algorithms, and data structure skills. This enhancement was chosen for my ePortfolio because it clearly demonstrates my ability to design and implement a structured robust software solution.  

The project showcases multiple important skills and competencies in software development. First is the data structure implementation that has a database use a vector to store the Character objects. Each section also has error handling in the case of try catches and throwing exceptions to handle any issues and other errors gracefully. The software design principles used for the CharacterDatabase class encapsulates all operations to managing characters, maintaining clean separation of concerns and a cohesive design. This enhancement aligns successfully with the course outcomes, which include designing and evaluating computing solutions using appropriate data structures and algorithms, and demonstrating innovative techniques, skills, and tools in computing practices to implement solutions that deliver value. 

Reflecting on the process the enhancement from the original python artifact to C++ provided valuable learning opportunities. It helped deepen my understanding of memory management, type safety, and structured error handling in C++. Implementing the comprehensive Character struct with multiple stats challenged me to design readable output and efficient CRUD operations. One key challenge was ensuring robust error handling while maintaining the clarity and performance. Overall this process reinforced the importance of thoughtful software design, testing, and documentation in building maintainable and forward thinking systems. .

## Artifact - [Algorithms and Data Structures](Artifacts/CourseBST.cpp)
For part 2 of the enhancement plan I added the Binary Sort Tree code from CS 300. The binary search tree was added to make efficient searching, insertion, updating, and deletion. This is added onto the existing infrastructure from the last enhancement of CRUD functionality. It also helps to demonstrate advanced data structure manipulation and algorithmic implementation in C++. The justification for why this was added as a part of the capstone project was to demonstrate my ability to design and implement complex data structures and algorithms in a structured, object-oriented way. Improvements from the original version include, replacing the simple crud storage model with a BST structure to efficiently organize and retrieve data, implementing a more robust exception handling, and organizing the program into modular files to better fit professional coding practices. These updates strengthened both the functionality and readability of the project, aligning it more closely with a more finished product. 

I believe this meets the course outcomes for this enhancement based on first demonstrating the ability to implement the BST algorithm into my existing code base. Next, I applied modularization, encapsulation, and error handling to reflect good software design principles. Finally I analyzed the limitations of the original crud only code and enhanced it with algorithmic efficiency and data abstraction. During the process of creating this part of the enhancement, having to break down the parts for handling the crud operations while maintaining the node handling in the BST. One challenge I faced was the delete operation, which had some errors with handling node deletion cases based on children. Overall this enhancement was a good test of my abilities to add on to an existing code base and connect algorithmic design with practical data management. 

## Artifact - [Databases](Artifacts/ProjectTwoDashboard-final.ipynb)
The third artifact of the enhancement plan is based around databases. I chose to use my artifact from CS 340 which used MongoDB as a database for the animals in the animal shelter. For my enhancement I uses SQLite 3 and had it store our character info inside of it. In this enhancement moving from a csv file, I was allowed persistent data storage, so the character data could be loaded, queried, and managed across sessions. The program automatically reads from characters.db and populates our BST, we are then able to interact with it with our previous CRUD interface to adjust it. Finally at the end I added a way to create a html report with all the information after the change, and create a chart based on the gun damage on the bottom. 

I selected the enhancement this way to demonstrate the ability to bridge algorithmic data structurees with real world data management systems. This database integration required me to understand the SQL schema and C++ database connectivity using SQLite3 to integrate a new data source different from the previous version. Some key components that highlight those skills are, the loadFromDB() function which uses prepared sql statements to query our data safely and efficiently with support from runtime exception handling. It also showcases the ability to integrate it into the system by taking the sql database and have it feed into our BST so I can integrate structured database output with a recursive in-memory data structure. This allowed me to convert this system from a local, static system to a scalable, persistent storage system. The enhancement meets the following course outcomes, first Data Management and Persistence via the SQLite integration for our storage, retrieval, and manipulation of persistent data. Next it handles Software development practices and error handling, with the modular integration of our database code being separated and then added in can allow for adjustments in the future, and the error handling and detection to ensure data import and handling is done safely. 

Reflecting on this enhancement, took some work getting the SQLite to work correctly and create the correct tables since the reading from the database is very strict with spelling and spaces. This helped me learn a new database structure compared to mongoDB to be able to compare and contrast their similarities in how you access them. During this time I also had to handle duplicates between the database and the BST and having to create a new function to handle duplicates. Overall this enhancment was a fun challenge to solidify my understanding of a more complex and real world system, connecting a backend to a front end and creating functions that can interact and share data between those systems.

##Self Assessment
