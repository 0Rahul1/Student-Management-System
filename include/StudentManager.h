#ifndef STUDENT_MANAGER_H
#define STUDENT_MANAGER_H

#include "Database.h"
#include "Student.h"
#include "Course.h"
#include "Attendance.h"
#include "Result.h"

#include <memory>
#include <vector>
#include <string>

/**
 * @brief Sorting criteria for student records.
 */
enum class SortCriteria {
    ByNameAscending,
    ByNameDescending,
    ByPercentageAscending,
    ByPercentageDescending
};

/**
 * @brief Controller class orchestrating business logic and user interactions.
 * 
 * Demonstrates:
 * - Composition: Has a Database member instance
 * - Function Overloading: Overloaded display, search, and sort routines
 * - Polymorphism: Invokes Person::displayInfo() polymorphically
 * - Exception Handling: Handles exceptions gracefully with user feedback
 */
class StudentManager {
private:
    std::unique_ptr<Database> m_db;

    // UI Helper routines
    void printHeader(const std::string& title) const;
    void printDivider() const;
    void printSuccess(const std::string& msg) const;
    void printError(const std::string& msg) const;
    void printWarning(const std::string& msg) const;
    void pauseConsole() const;
    int getValidInteger(const std::string& prompt, int minVal = 0, int maxVal = 1000000) const;
    double getValidDouble(const std::string& prompt, double minVal = 0.0, double maxVal = 100000.0) const;
    std::string getValidString(const std::string& prompt, bool allowEmpty = false) const;
    bool confirmAction(const std::string& prompt) const;

public:
    StudentManager();
    explicit StudentManager(std::unique_ptr<Database> db);
    ~StudentManager();

    // Application entry point
    void run();

    // Menus
    void showMainMenu();
    void studentManagementMenu();
    void courseManagementMenu();
    void attendanceManagementMenu();
    void resultManagementMenu();
    void searchAndFilterMenu();
    void showDashboard();

    // Module A: Student Operations
    void addStudent();
    void displayAllStudents();
    void searchStudentById();
    void searchStudentByName();
    void updateStudent();
    void deleteStudent();

    // Module B: Course Operations
    void addCourse();
    void displayAllCourses();
    void assignCourseToStudent();
    void updateCourse();

    // Module C: Attendance Operations
    void markAttendance();
    void viewAttendanceRecords();
    void calculateAttendancePercentage();

    // Module D: Result Operations
    void addStudentMarks();
    void displayStudentResult();
    void displayAllResultRankings();

    // Module E: Search and Filter Operations
    void filterStudentsByCourse();
    void sortStudentsMenu();

    // Function Overloading Demonstration
    void displayStudent(const Student& student) const;
    void displayStudents(const std::vector<Student>& students) const;
    bool searchStudent(const std::string& studentId);
    bool searchStudent(const std::string& name, bool searchByName);

    // Sorting implementation
    void sortStudents(SortCriteria criteria);
};

#endif // STUDENT_MANAGER_H
