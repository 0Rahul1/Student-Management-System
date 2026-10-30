#ifndef DATABASE_H
#define DATABASE_H

#include "Student.h"
#include "Course.h"
#include "Attendance.h"
#include "Result.h"
#include "Exceptions.h"

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <utility>

/**
 * @brief Dashboard statistics structure for high-level summaries.
 */
struct DashboardStats {
    int totalStudents = 0;
    int totalCourses = 0;
    int totalAttendanceRecords = 0;
    double averagePercentage = 0.0;
};

/**
 * @brief Database class managing persistent storage with MongoDB.
 * 
 * Demonstrates:
 * - Encapsulation & Abstraction: Uses PIMPL (Pointer to Implementation) idiom
 *   to completely isolate MongoDB C++ driver types and headers from consumers.
 * - Exception Handling: Translates driver exceptions into domain exceptions.
 * - Dynamic Connection: Supports both Localhost MongoDB and MongoDB Atlas cloud URIs.
 */
class Database {
private:
    std::string m_uriString;
    std::string m_dbName;
    bool m_connected;

    // Forward declaration of PIMPL structure
    struct Impl;
    std::unique_ptr<Impl> m_impl;

    // Internal helper to load configuration
    void loadConfig();

public:
    Database();
    explicit Database(std::string uri, std::string dbName = "StudentManagementDB");
    ~Database();

    // Prevent copy, allow move (RAII)
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&&) noexcept;
    Database& operator=(Database&&) noexcept;

    // Connection lifecycle
    bool connect();
    void disconnect();
    bool isConnected() const;

    // Index creation for uniqueness and query speed
    void createIndexes();

    // Student CRUD
    bool insertStudent(const Student& student);
    std::vector<Student> getAllStudents();
    std::optional<Student> getStudentById(const std::string& studentId);
    std::vector<Student> getStudentsByName(const std::string& name);
    std::vector<Student> getStudentsByCourse(const std::string& course);
    bool updateStudent(const Student& student);
    bool deleteStudent(const std::string& studentId);
    bool studentExists(const std::string& studentId);

    // Course CRUD
    bool insertCourse(const Course& course);
    std::vector<Course> getAllCourses();
    std::optional<Course> getCourseById(const std::string& courseId);
    bool updateCourse(const Course& course);
    bool courseExists(const std::string& courseId);

    // Attendance Management
    bool recordAttendance(const Attendance& attendance);
    std::vector<Attendance> getAttendanceByStudent(const std::string& studentId);
    std::pair<int, int> getAttendanceStats(const std::string& studentId);

    // Result Management
    bool recordResult(const Result& result);
    std::optional<Result> getResultByStudent(const std::string& studentId);
    std::vector<Result> getAllResults();

    // Dashboard Statistics Aggregation
    DashboardStats getDashboardStats();

    // Global driver initialization
    static void initializeDriver();
};

#endif // DATABASE_H
