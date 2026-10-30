#include "Database.h"
#include "Exceptions.h"

#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>

#if __has_include(<mongocxx/client.hpp>)
    #define HAS_MONGOCXX 1
    #include <mongocxx/client.hpp>
    #include <mongocxx/instance.hpp>
    #include <mongocxx/uri.hpp>
    #include <mongocxx/database.hpp>
    #include <mongocxx/collection.hpp>
    #include <mongocxx/exception/exception.hpp>
    #include <mongocxx/pipeline.hpp>
    #include <mongocxx/options/index.hpp>
    #include <mongocxx/options/update.hpp>
    #include <bsoncxx/builder/stream/document.hpp>
    #include <bsoncxx/builder/stream/array.hpp>
    #include <bsoncxx/builder/stream/helpers.hpp>
    #include <bsoncxx/builder/basic/document.hpp>
    #include <bsoncxx/builder/basic/kvp.hpp>
    #include <bsoncxx/types.hpp>
    #include <bsoncxx/json.hpp>
#else
    #define HAS_MONGOCXX 0
#endif

// Private Implementation (PIMPL) structure
struct Database::Impl {
#if HAS_MONGOCXX
    std::unique_ptr<mongocxx::client> client;
    std::unique_ptr<mongocxx::database> db;
    std::unique_ptr<mongocxx::collection> studentsCol;
    std::unique_ptr<mongocxx::collection> coursesCol;
    std::unique_ptr<mongocxx::collection> attendanceCol;
    std::unique_ptr<mongocxx::collection> resultsCol;
#endif
};

#if HAS_MONGOCXX
// Static mongocxx instance (initialized once per process lifetime)
static std::unique_ptr<mongocxx::instance> s_driverInstance = nullptr;
#endif

// Default constructor
Database::Database()
    : m_uriString(""), m_dbName("StudentManagementDB"), m_connected(false),
      m_impl(std::make_unique<Impl>()) {
    loadConfig();
}

// Parameterized constructor
Database::Database(std::string uri, std::string dbName)
    : m_uriString(std::move(uri)), m_dbName(std::move(dbName)), m_connected(false),
      m_impl(std::make_unique<Impl>()) {
    if (m_uriString.empty()) {
        loadConfig();
    }
}

// Destructor & Move semantics
Database::~Database() {
    disconnect();
}

Database::Database(Database&&) noexcept = default;
Database& Database::operator=(Database&&) noexcept = default;

// Global driver instance initialization
void Database::initializeDriver() {
#if HAS_MONGOCXX
    if (!s_driverInstance) {
        s_driverInstance = std::make_unique<mongocxx::instance>();
    }
#endif
}

// Load connection URI from .env file or environment variable
void Database::loadConfig() {
    // 1. Check environment variable first
    const char* envUri = std::getenv("MONGODB_URI");
    if (envUri != nullptr && std::string(envUri).length() > 0) {
        m_uriString = envUri;
        return;
    }

    // 2. Check for .env file
    std::ifstream envFile(".env");
    if (envFile.is_open()) {
        std::string line;
        while (std::getline(envFile, line)) {
            size_t start = line.find_first_not_of(" \t\r\n");
            if (start == std::string::npos || line[start] == '#') continue;
            
            size_t delim = line.find('=');
            if (delim != std::string::npos) {
                std::string key = line.substr(start, delim - start);
                std::string val = line.substr(delim + 1);
                size_t valStart = val.find_first_not_of(" \t\"'");
                size_t valEnd = val.find_last_not_of(" \t\r\n\"'");
                if (valStart != std::string::npos && valEnd != std::string::npos) {
                    val = val.substr(valStart, valEnd - valStart + 1);
                }
                if (key == "MONGODB_URI" && !val.empty()) {
                    m_uriString = val;
                    return;
                }
            }
        }
    }

    // 3. Fallback to local MongoDB URI
    m_uriString = "mongodb://localhost:27017";
}

bool Database::connect() {
#if HAS_MONGOCXX
    try {
        initializeDriver();

        mongocxx::uri uri{m_uriString};
        m_impl->client = std::make_unique<mongocxx::client>(uri);

        // Ping database to verify live connectivity
        auto admin = (*m_impl->client)["admin"];
        bsoncxx::builder::stream::document ping_cmd;
        ping_cmd << "ping" << 1;
        admin.run_command(ping_cmd.view());

        // Connect to database and collections
        m_impl->db = std::make_unique<mongocxx::database>((*m_impl->client)[m_dbName]);
        m_impl->studentsCol = std::make_unique<mongocxx::collection>((*m_impl->db)["students"]);
        m_impl->coursesCol = std::make_unique<mongocxx::collection>((*m_impl->db)["courses"]);
        m_impl->attendanceCol = std::make_unique<mongocxx::collection>((*m_impl->db)["attendance"]);
        m_impl->resultsCol = std::make_unique<mongocxx::collection>((*m_impl->db)["results"]);

        m_connected = true;

        // Ensure unique and compound indexes are active
        createIndexes();
        return true;
    } catch (const mongocxx::exception& e) {
        m_connected = false;
        throw DatabaseException(std::string("MongoDB Driver Exception: ") + e.what() + 
                                " (URI: " + m_uriString + ")");
    } catch (const std::exception& e) {
        m_connected = false;
        throw DatabaseException(std::string("Connection failure: ") + e.what() + 
                                " (Ensure MongoDB is running or check your URI in .env)");
    }
#else
    throw DatabaseException("MongoDB C++ driver headers (mongocxx) are being configured. Please ensure mongocxx is installed via vcpkg.");
#endif
}

void Database::disconnect() {
#if HAS_MONGOCXX
    if (m_impl) {
        m_impl->studentsCol.reset();
        m_impl->coursesCol.reset();
        m_impl->attendanceCol.reset();
        m_impl->resultsCol.reset();
        m_impl->db.reset();
        m_impl->client.reset();
    }
#endif
    m_connected = false;
}

bool Database::isConnected() const {
    return m_connected;
}

void Database::createIndexes() {
#if HAS_MONGOCXX
    if (!m_connected || !m_impl || !m_impl->studentsCol) return;

    try {
        // Unique index on students.student_id
        bsoncxx::builder::stream::document studentIdx;
        studentIdx << "student_id" << 1;
        mongocxx::options::index uniqueOpt{};
        uniqueOpt.unique(true);
        m_impl->studentsCol->create_index(studentIdx.view(), uniqueOpt);

        // Unique index on courses.course_id
        bsoncxx::builder::stream::document courseIdx;
        courseIdx << "course_id" << 1;
        m_impl->coursesCol->create_index(courseIdx.view(), uniqueOpt);

        // Compound index on attendance (student_id, date)
        bsoncxx::builder::stream::document attIdx;
        attIdx << "student_id" << 1 << "date" << 1;
        m_impl->attendanceCol->create_index(attIdx.view());

        // Index on results.student_id
        bsoncxx::builder::stream::document resIdx;
        resIdx << "student_id" << 1;
        m_impl->resultsCol->create_index(resIdx.view());
    } catch (...) {
        // Non-fatal if index already exists
    }
#endif
}

// ==========================================
// STUDENT CRUD OPERATIONS
// ==========================================

bool Database::insertStudent(const Student& student) {
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->studentsCol) throw DatabaseException("Database is not connected.");

    if (studentExists(student.getId())) {
        throw DuplicateRecordException("Student with ID '" + student.getId() + "' already exists in MongoDB.");
    }

    try {
        bsoncxx::builder::stream::document doc;
        doc << "student_id" << student.getId()
            << "name" << student.getName()
            << "age" << student.getAge()
            << "email" << student.getEmail()
            << "phone" << student.getPhone()
            << "gender" << student.getGender()
            << "course" << student.getCourse()
            << "semester" << student.getSemester()
            << "enrollment_date" << student.getEnrollmentDate();

        auto res = m_impl->studentsCol->insert_one(doc.view());
        return res ? true : false;
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to insert student: " + std::string(e.what()));
    }
#else
    return false;
#endif
}

std::vector<Student> Database::getAllStudents() {
    std::vector<Student> list;
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->studentsCol) throw DatabaseException("Database is not connected.");

    try {
        auto cursor = m_impl->studentsCol->find({});
        for (const auto& doc : cursor) {
            std::string id = doc["student_id"] ? std::string(doc["student_id"].get_string().value) : "";
            std::string name = doc["name"] ? std::string(doc["name"].get_string().value) : "";
            int age = doc["age"] ? doc["age"].get_int32().value : 18;
            std::string email = doc["email"] ? std::string(doc["email"].get_string().value) : "";
            std::string phone = doc["phone"] ? std::string(doc["phone"].get_string().value) : "";
            std::string gender = doc["gender"] ? std::string(doc["gender"].get_string().value) : "";
            std::string course = doc["course"] ? std::string(doc["course"].get_string().value) : "";
            int semester = doc["semester"] ? doc["semester"].get_int32().value : 1;
            std::string enrollDate = doc["enrollment_date"] ? std::string(doc["enrollment_date"].get_string().value) : "";

            list.emplace_back(id, name, age, email, phone, gender, course, semester, enrollDate);
        }
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to fetch all students: " + std::string(e.what()));
    }
#endif
    return list;
}

std::optional<Student> Database::getStudentById(const std::string& studentId) {
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->studentsCol) throw DatabaseException("Database is not connected.");

    try {
        bsoncxx::builder::stream::document filter;
        filter << "student_id" << studentId;

        auto docOpt = m_impl->studentsCol->find_one(filter.view());
        if (docOpt) {
            auto doc = docOpt->view();
            std::string id = std::string(doc["student_id"].get_string().value);
            std::string name = std::string(doc["name"].get_string().value);
            int age = doc["age"].get_int32().value;
            std::string email = std::string(doc["email"].get_string().value);
            std::string phone = std::string(doc["phone"].get_string().value);
            std::string gender = std::string(doc["gender"].get_string().value);
            std::string course = std::string(doc["course"].get_string().value);
            int semester = doc["semester"].get_int32().value;
            std::string enrollDate = std::string(doc["enrollment_date"].get_string().value);

            return Student(id, name, age, email, phone, gender, course, semester, enrollDate);
        }
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to query student by ID: " + std::string(e.what()));
    }
#endif
    return std::nullopt;
}

std::vector<Student> Database::getStudentsByName(const std::string& name) {
    std::vector<Student> list;
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->studentsCol) throw DatabaseException("Database is not connected.");

    try {
        bsoncxx::builder::stream::document filter;
        filter << "name" << bsoncxx::builder::stream::open_document
               << "$regex" << name
               << "$options" << "i"
               << bsoncxx::builder::stream::close_document;

        auto cursor = m_impl->studentsCol->find(filter.view());
        for (const auto& doc : cursor) {
            std::string id = std::string(doc["student_id"].get_string().value);
            std::string sname = std::string(doc["name"].get_string().value);
            int age = doc["age"].get_int32().value;
            std::string email = std::string(doc["email"].get_string().value);
            std::string phone = std::string(doc["phone"].get_string().value);
            std::string gender = std::string(doc["gender"].get_string().value);
            std::string course = std::string(doc["course"].get_string().value);
            int semester = doc["semester"].get_int32().value;
            std::string enrollDate = std::string(doc["enrollment_date"].get_string().value);

            list.emplace_back(id, sname, age, email, phone, gender, course, semester, enrollDate);
        }
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to query students by name: " + std::string(e.what()));
    }
#endif
    return list;
}

std::vector<Student> Database::getStudentsByCourse(const std::string& course) {
    std::vector<Student> list;
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->studentsCol) throw DatabaseException("Database is not connected.");

    try {
        bsoncxx::builder::stream::document filter;
        filter << "course" << course;

        auto cursor = m_impl->studentsCol->find(filter.view());
        for (const auto& doc : cursor) {
            std::string id = std::string(doc["student_id"].get_string().value);
            std::string name = std::string(doc["name"].get_string().value);
            int age = doc["age"].get_int32().value;
            std::string email = std::string(doc["email"].get_string().value);
            std::string phone = std::string(doc["phone"].get_string().value);
            std::string gender = std::string(doc["gender"].get_string().value);
            std::string scourse = std::string(doc["course"].get_string().value);
            int semester = doc["semester"].get_int32().value;
            std::string enrollDate = std::string(doc["enrollment_date"].get_string().value);

            list.emplace_back(id, name, age, email, phone, gender, scourse, semester, enrollDate);
        }
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to filter students by course: " + std::string(e.what()));
    }
#endif
    return list;
}

bool Database::updateStudent(const Student& student) {
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->studentsCol) throw DatabaseException("Database is not connected.");

    try {
        bsoncxx::builder::stream::document filter;
        filter << "student_id" << student.getId();

        bsoncxx::builder::stream::document updateDoc;
        updateDoc << "$set" << bsoncxx::builder::stream::open_document
                  << "name" << student.getName()
                  << "age" << student.getAge()
                  << "email" << student.getEmail()
                  << "phone" << student.getPhone()
                  << "gender" << student.getGender()
                  << "course" << student.getCourse()
                  << "semester" << student.getSemester()
                  << "enrollment_date" << student.getEnrollmentDate()
                  << bsoncxx::builder::stream::close_document;

        auto res = m_impl->studentsCol->update_one(filter.view(), updateDoc.view());
        return (res && res->modified_count() > 0);
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to update student: " + std::string(e.what()));
    }
#else
    return false;
#endif
}

bool Database::deleteStudent(const std::string& studentId) {
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->studentsCol) throw DatabaseException("Database is not connected.");

    try {
        bsoncxx::builder::stream::document filter;
        filter << "student_id" << studentId;

        auto res = m_impl->studentsCol->delete_one(filter.view());

        // Cascade delete related records
        m_impl->attendanceCol->delete_many(filter.view());
        m_impl->resultsCol->delete_many(filter.view());

        return (res && res->deleted_count() > 0);
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to delete student: " + std::string(e.what()));
    }
#else
    return false;
#endif
}

bool Database::studentExists(const std::string& studentId) {
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->studentsCol) return false;
    try {
        bsoncxx::builder::stream::document filter;
        filter << "student_id" << studentId;
        return m_impl->studentsCol->count_documents(filter.view()) > 0;
    } catch (...) {
        return false;
    }
#else
    return false;
#endif
}

// ==========================================
// COURSE CRUD OPERATIONS
// ==========================================

bool Database::insertCourse(const Course& course) {
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->coursesCol) throw DatabaseException("Database is not connected.");

    if (courseExists(course.getCourseId())) {
        throw DuplicateRecordException("Course with ID '" + course.getCourseId() + "' already exists.");
    }

    try {
        bsoncxx::builder::stream::document doc;
        doc << "course_id" << course.getCourseId()
            << "course_name" << course.getCourseName()
            << "department" << course.getDepartment()
            << "credits" << course.getCredits()
            << "instructor" << course.getInstructor();

        auto res = m_impl->coursesCol->insert_one(doc.view());
        return res ? true : false;
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to insert course: " + std::string(e.what()));
    }
#else
    return false;
#endif
}

std::vector<Course> Database::getAllCourses() {
    std::vector<Course> list;
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->coursesCol) throw DatabaseException("Database is not connected.");

    try {
        auto cursor = m_impl->coursesCol->find({});
        for (const auto& doc : cursor) {
            std::string id = std::string(doc["course_id"].get_string().value);
            std::string name = std::string(doc["course_name"].get_string().value);
            std::string dept = std::string(doc["department"].get_string().value);
            int credits = doc["credits"].get_int32().value;
            std::string inst = std::string(doc["instructor"].get_string().value);

            list.emplace_back(id, name, dept, credits, inst);
        }
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to fetch courses: " + std::string(e.what()));
    }
#endif
    return list;
}

std::optional<Course> Database::getCourseById(const std::string& courseId) {
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->coursesCol) throw DatabaseException("Database is not connected.");

    try {
        bsoncxx::builder::stream::document filter;
        filter << "course_id" << courseId;

        auto docOpt = m_impl->coursesCol->find_one(filter.view());
        if (docOpt) {
            auto doc = docOpt->view();
            std::string id = std::string(doc["course_id"].get_string().value);
            std::string name = std::string(doc["course_name"].get_string().value);
            std::string dept = std::string(doc["department"].get_string().value);
            int credits = doc["credits"].get_int32().value;
            std::string inst = std::string(doc["instructor"].get_string().value);

            return Course(id, name, dept, credits, inst);
        }
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to query course by ID: " + std::string(e.what()));
    }
#endif
    return std::nullopt;
}

bool Database::updateCourse(const Course& course) {
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->coursesCol) throw DatabaseException("Database is not connected.");

    try {
        bsoncxx::builder::stream::document filter;
        filter << "course_id" << course.getCourseId();

        bsoncxx::builder::stream::document updateDoc;
        updateDoc << "$set" << bsoncxx::builder::stream::open_document
                  << "course_name" << course.getCourseName()
                  << "department" << course.getDepartment()
                  << "credits" << course.getCredits()
                  << "instructor" << course.getInstructor()
                  << bsoncxx::builder::stream::close_document;

        auto res = m_impl->coursesCol->update_one(filter.view(), updateDoc.view());
        return (res && res->modified_count() > 0);
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to update course: " + std::string(e.what()));
    }
#else
    return false;
#endif
}

bool Database::courseExists(const std::string& courseId) {
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->coursesCol) return false;
    try {
        bsoncxx::builder::stream::document filter;
        filter << "course_id" << courseId;
        return m_impl->coursesCol->count_documents(filter.view()) > 0;
    } catch (...) {
        return false;
    }
#else
    return false;
#endif
}

// ==========================================
// ATTENDANCE MANAGEMENT
// ==========================================

bool Database::recordAttendance(const Attendance& attendance) {
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->attendanceCol) throw DatabaseException("Database is not connected.");

    if (!studentExists(attendance.getStudentId())) {
        throw RecordNotFoundException("Cannot record attendance: Student '" + attendance.getStudentId() + "' not found.");
    }

    try {
        bsoncxx::builder::stream::document doc;
        doc << "student_id" << attendance.getStudentId()
            << "course_id" << attendance.getCourseId()
            << "date" << attendance.getDate()
            << "status" << attendance.getStatus()
            << "remarks" << attendance.getRemarks();

        auto res = m_impl->attendanceCol->insert_one(doc.view());
        return res ? true : false;
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to record attendance: " + std::string(e.what()));
    }
#else
    return false;
#endif
}

std::vector<Attendance> Database::getAttendanceByStudent(const std::string& studentId) {
    std::vector<Attendance> list;
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->attendanceCol) throw DatabaseException("Database is not connected.");

    try {
        bsoncxx::builder::stream::document filter;
        filter << "student_id" << studentId;

        auto cursor = m_impl->attendanceCol->find(filter.view());
        for (const auto& doc : cursor) {
            std::string sid = std::string(doc["student_id"].get_string().value);
            std::string cid = std::string(doc["course_id"].get_string().value);
            std::string date = std::string(doc["date"].get_string().value);
            std::string status = std::string(doc["status"].get_string().value);
            std::string remarks = doc["remarks"] ? std::string(doc["remarks"].get_string().value) : "";

            list.emplace_back(sid, cid, date, status, remarks);
        }
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to query student attendance: " + std::string(e.what()));
    }
#endif
    return list;
}

std::pair<int, int> Database::getAttendanceStats(const std::string& studentId) {
    int presentCount = 0;
    int totalCount = 0;
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->attendanceCol) throw DatabaseException("Database is not connected.");

    try {
        bsoncxx::builder::stream::document filter;
        filter << "student_id" << studentId;

        auto cursor = m_impl->attendanceCol->find(filter.view());
        for (const auto& doc : cursor) {
            std::string status = std::string(doc["status"].get_string().value);
            ++totalCount;
            if (status == "Present" || status == "present") {
                ++presentCount;
            }
        }
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to calculate attendance statistics: " + std::string(e.what()));
    }
#endif
    return {presentCount, totalCount};
}

// ==========================================
// RESULT MANAGEMENT
// ==========================================

bool Database::recordResult(const Result& result) {
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->resultsCol) throw DatabaseException("Database is not connected.");

    if (!studentExists(result.getStudentId())) {
        throw RecordNotFoundException("Cannot record result: Student '" + result.getStudentId() + "' not found.");
    }

    try {
        bsoncxx::builder::stream::document doc;
        doc << "student_id" << result.getStudentId()
            << "course_id" << result.getCourseId()
            << "total_marks" << result.getTotalMarks()
            << "max_marks" << result.getMaxPossibleMarks()
            << "percentage" << result.getPercentage()
            << "grade" << result.getGrade();

        auto arr = doc << "subjects" << bsoncxx::builder::stream::open_array;
        for (const auto& sub : result.getSubjects()) {
            arr = arr << bsoncxx::builder::stream::open_document
                      << "subject_name" << sub.subjectName
                      << "marks_obtained" << sub.marksObtained
                      << "max_marks" << sub.maxMarks
                      << bsoncxx::builder::stream::close_document;
        }
        doc = arr << bsoncxx::builder::stream::close_array;

        bsoncxx::builder::stream::document filter;
        filter << "student_id" << result.getStudentId()
               << "course_id" << result.getCourseId();

        mongocxx::options::update updateOpts{};
        updateOpts.upsert(true);

        bsoncxx::builder::stream::document updateDoc;
        updateDoc << "$set" << doc.view();

        auto res = m_impl->resultsCol->update_one(filter.view(), updateDoc.view(), updateOpts);
        return res ? true : false;
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to record student result: " + std::string(e.what()));
    }
#else
    return false;
#endif
}

std::optional<Result> Database::getResultByStudent(const std::string& studentId) {
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->resultsCol) throw DatabaseException("Database is not connected.");

    try {
        bsoncxx::builder::stream::document filter;
        filter << "student_id" << studentId;

        auto docOpt = m_impl->resultsCol->find_one(filter.view());
        if (docOpt) {
            auto doc = docOpt->view();
            std::string sid = std::string(doc["student_id"].get_string().value);
            std::string cid = std::string(doc["course_id"].get_string().value);

            Result res(sid, cid);

            if (doc["subjects"] && doc["subjects"].type() == bsoncxx::type::k_array) {
                auto arr = doc["subjects"].get_array().value;
                for (const auto& el : arr) {
                    auto subDoc = el.get_document().value;
                    std::string sname = std::string(subDoc["subject_name"].get_string().value);
                    double marks = subDoc["marks_obtained"].get_double().value;
                    double maxM = subDoc["max_marks"].get_double().value;
                    res.addSubjectMark(sname, marks, maxM);
                }
            }
            res.computeTotals();
            return res;
        }
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to query student result: " + std::string(e.what()));
    }
#endif
    return std::nullopt;
}

std::vector<Result> Database::getAllResults() {
    std::vector<Result> list;
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->resultsCol) throw DatabaseException("Database is not connected.");

    try {
        auto cursor = m_impl->resultsCol->find({});
        for (const auto& doc : cursor) {
            std::string sid = std::string(doc["student_id"].get_string().value);
            std::string cid = std::string(doc["course_id"].get_string().value);

            Result res(sid, cid);
            if (doc["subjects"] && doc["subjects"].type() == bsoncxx::type::k_array) {
                auto arr = doc["subjects"].get_array().value;
                for (const auto& el : arr) {
                    auto subDoc = el.get_document().value;
                    std::string sname = std::string(subDoc["subject_name"].get_string().value);
                    double marks = subDoc["marks_obtained"].get_double().value;
                    double maxM = subDoc["max_marks"].get_double().value;
                    res.addSubjectMark(sname, marks, maxM);
                }
            }
            res.computeTotals();
            list.push_back(res);
        }
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to fetch all results: " + std::string(e.what()));
    }
#endif
    return list;
}

// ==========================================
// DASHBOARD STATS AGGREGATION
// ==========================================

DashboardStats Database::getDashboardStats() {
    DashboardStats stats;
#if HAS_MONGOCXX
    if (!m_connected || !m_impl->studentsCol) throw DatabaseException("Database is not connected.");

    try {
        stats.totalStudents = static_cast<int>(m_impl->studentsCol->count_documents({}));
        stats.totalCourses = static_cast<int>(m_impl->coursesCol->count_documents({}));
        stats.totalAttendanceRecords = static_cast<int>(m_impl->attendanceCol->count_documents({}));

        mongocxx::pipeline pipe;
        pipe.group(bsoncxx::builder::basic::make_document(
            bsoncxx::builder::basic::kvp("_id", bsoncxx::types::b_null{}),
            bsoncxx::builder::basic::kvp("avgPercentage", 
                bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$avg", "$percentage")))
        ));

        auto cursor = m_impl->resultsCol->aggregate(pipe);
        for (const auto& doc : cursor) {
            if (doc["avgPercentage"]) {
                if (doc["avgPercentage"].type() == bsoncxx::type::k_double) {
                    stats.averagePercentage = doc["avgPercentage"].get_double().value;
                } else if (doc["avgPercentage"].type() == bsoncxx::type::k_int32) {
                    stats.averagePercentage = static_cast<double>(doc["avgPercentage"].get_int32().value);
                }
            }
        }
    } catch (const mongocxx::exception& e) {
        throw DatabaseException("Failed to aggregate dashboard statistics: " + std::string(e.what()));
    }
#endif
    return stats;
}
