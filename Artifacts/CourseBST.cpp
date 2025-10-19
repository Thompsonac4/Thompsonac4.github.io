//============================================================================
// Name        : CourseBST.cpp
// Author      : Austin Thompson
// Version     : 1.0
// Description : Final Project Courses Binary Search Tree
//============================================================================
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

// Structure to represent a course
struct Course {
	// Course Identifier
	std::string courseNumber;
	// Course Title
	std::string courseTitle;
	// List of Prerequisites
	std::vector<std::string> prerequisites;

};

// Structure for a Binary Search Tree node
struct TreeNode {
	Course course;
	TreeNode* left;
	TreeNode* right;

	//Constructor
	TreeNode(Course c) : course(c), left(nullptr), right(nullptr) {}

};

//============================================================================
// Course Binary Search Tree class definition
//============================================================================
class CourseBST {
private:
	TreeNode* root;

	/*
	* Recursively insert a course into the BST
	* 
	* @param node - Current node reference
	* @param course - Course to insert
	*/
	void insertNode(TreeNode*& node, Course course) {
		// Insert Node here
		if (!node) {
			node = new TreeNode(course);
		}
		// Traverse Left
		else if (course.courseNumber < node->course.courseNumber) {
			insertNode(node->left, course);
		}
		// Traverse Right
		else {
			insertNode(node->right, course);
		}
	}

	/*
	* Recursively print courses in in-order traversal
	* 
	* @param node - Current node in the tree
	*/
	void printInOrder(TreeNode* node) {
		if (!node) return;
		printInOrder(node->left);
		std::cout << node->course.courseNumber << ": " << node->course.courseTitle << std::endl;
		printInOrder(node->right);
	}

	/*
	* Recursively find a course by course number
	* 
	* @param node - Current node in the tree
	* @param courseNumber - The course number
	* @return Pointer to the course
	*/
	Course* find(TreeNode* node, const std::string& courseNumber) {
		if (!node) return nullptr;
		if (courseNumber == node->course.courseNumber) return &node->course;
		if (courseNumber < node->course.courseNumber) return find(node->left, courseNumber);
		return find(node->right, courseNumber);

	}
public:
	// Constructor
	CourseBST() : root(nullptr) {}

	/*
	* Insert a course into the BST
	* 
	* @param course - the course that is inserted
	*/
	void insert(Course course) {
		insertNode(root, course);
	}

	/*
	* Print all courses in sorted order 
	*/
	void printAllCourses() {
		printInOrder(root);
	}

	/*
	* Find and return a course by its number.
	* 
	* @param courseNumber - The course number
	* @return Pointer to the course
	*/
	Course* findCourse(const std::string& courseNumber) {
		return find(root, courseNumber);
	}


};

/*
* Load course data from a CSV file and insert into the BST.
* 
* @param filename - Path to the CSV file
* @param bst - Reference to the CourseBST to populate
*/
void loadCoursesFromFile(const std::string& filename, CourseBST& bst) {
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cout << "Error: File could not be opened." << std::endl;
		return;
	}

	std::string line;
	while (getline(file, line)) {
		std::stringstream ss(line);
		std::string token;
		Course course;
		// Read course number
		if (!std::getline(ss, token, ',')) continue;
		course.courseNumber = token;

		// Read course title
		if (!std::getline(ss, token, ',')) continue;
		course.courseTitle = token;
		
		// Read prerequisites, remaining tokens
		while (getline(ss, token, ',')) {
			course.prerequisites.push_back(token);
		}

		//Insert the course into BST
		bst.insert(course);
	}
	file.close();
	std::cout << "Courses loaded successfully." << std::endl;
}
/*
* Print information about a specific course and its prerequisites
* 
* @param bst - Reference to the CourseBST to populate
* @param courseNumber - The course number
*/
void printCourseInfo(CourseBST& bst, const std::string& courseNumber) {
	Course* course = bst.findCourse(courseNumber);
	// If courses not found
	if (!course) {
		std::cout << "Course not found." << std::endl;
		return;
	}

	std::cout << "\nCourse Number: " << course->courseNumber << std::endl;
	std::cout << "Course Title: " << course->courseTitle << std::endl;

	//Print Prerequisites
	if (course->prerequisites.empty()) {
		std::cout << "Prerequisites: None" << std::endl;

	}
	else {
		std::cout << "Prerequisites: " << std::endl;
		for (const std::string& prereq : course->prerequisites) {
			Course* prereqCourse = bst.findCourse(prereq);
			if (prereqCourse) {
				std::cout << "- " << prereqCourse->courseNumber << ": " << prereqCourse->courseTitle << std::endl;
			}
			else {
				std::cout << "- " << prereq << " (Not Found)" << std::endl;
			}
		}
	}
}

//============================================================================
// Main Method
//============================================================================
int main() {
	CourseBST bst;
	std::string filename = "Test.csv";
	bool dataloaded = false;

	while (true) {
		//Menu Options
		std::cout << "\n1. Load course data" << std::endl;
		std::cout << "2. Display all courses" << std::endl;
		std::cout << "3. Search for a course" << std::endl;
		std::cout << "9. Exit" << std::endl;
		std::cout << "Enter your choice: " << std::endl;

		int choice;
		std::cin >> choice;
		std::cin.ignore();
		// Load File
		if (choice == 1) {
			
			loadCoursesFromFile(filename, bst);
			dataloaded = true;
		}
		// Show All course data
		else if (choice == 2) {
			if (!dataloaded) {
				std::cout << "Please load course data first." << std::endl;
			}
			else {
				bst.printAllCourses();
			}
		}
		// Show prerequisites for select course
		else if (choice == 3) {
			if (!dataloaded) {
				std::cout << "Please load course data first." << std::endl;
			}
			else {
				std::string courseNum;
				std::cout << "Enter course number: ";
				std::cin >> courseNum;
				printCourseInfo(bst, courseNum);
			}
		}
		// Exit
		else if (choice == 9) {
			std::cout << "Exiting." << std::endl;
			break;
		}
		// Wrong input
		else {
			std::cout << "Invalid Input. Try Again" << std::endl;
		}
	}
}