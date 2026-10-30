#include "StudentManager.h"
#include "Exceptions.h"

#include <iostream>
#include <iomanip>
#include <limits>
#include <algorithm>
#include <map>

// Constructors & Destructor
StudentManager::StudentManager()
    : m_db(std::make_unique<Database>()) {
}

StudentManager::StudentManager(std::unique_ptr<Database> db)
    : m_db(std::move(db)) {
}

StudentManager::~StudentManager() = default;

// ==========================================
// UI FORMATTING & INPUT HELPERS
// ==========================================

void StudentManager::printHeader(const std::string& title) const {
    std::cout << "\n=============================================================\n";
    std::cout << "  " << title << "\n";
    std::cout << "=============================================================\n";
}

void StudentManager::printDivider() const {
    std::cout << "-------------------------------------------------------------\n";
}

void StudentManager::printSuccess(const std::string& msg) const {
    std::cout << "\n[SUCCESS] " << msg << "\n";
}

void StudentManager::printError(const std::string& msg) const {
    std::cout << "\n[ERROR] " << msg << "\n";
}

void StudentManager::printWarning(const std::string& msg) const {
    std::cout << "\n[WARNING] " << msg << "\n";
}

void StudentManager::pauseConsole() const {
    std::cout << "\nPress Enter to continue...";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int StudentManager::getValidInteger(const std::string& prompt, int minVal, int maxVal) const {
    int val = 0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            if (val >= minVal && val <= maxVal) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return val;
            } else {
                std::cout << "Value must be between " << minVal << " and " << maxVal << ". Please retry.\n";
            }
        } else {
            std::cout << "Invalid input. Please enter a valid number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

double StudentManager::getValidDouble(const std::string& prompt, double minVal, double maxVal) const {
    double val = 0.0;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            if (val >= minVal && val <= maxVal) {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return val;
            } else {
                std::cout << "Value must be between " << minVal << " and " << maxVal << ". Please retry.\n";
            }
        } else {
            std::cout << "Invalid input. Please enter a valid decimal number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

std::string StudentManager::getValidString(const std::string& prompt, bool allowEmpty) const {
    std::string line;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, line);
        // Trim leading and trailing spaces
        size_t first = line.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            if (allowEmpty) return "";
            std::cout << "Input cannot be empty or only whitespace. Please retry.\n";
            continue;
        }
        size_t last = line.find_last_not_of(" \t\r\n");
        return line.substr(first, (last - first + 1));
    }
}

bool StudentManager::confirmAction(const std::string& prompt) const {
    std::cout << prompt << " (y/n): ";
    std::string response;
    std::getline(std::cin, response);
    return (!response.empty() && (response[0] == 'y' || response[0] == 'Y'));
}

// ==========================================
// MAIN CONTROLLER RUN LOOP
// ==========================================

void StudentManager::run() {
    printHeader("COLLEGE STUDENT MANAGEMENT SYSTEM (C++ / MongoDB)");
    std::cout << "Initializing database connection...\n";

    try {
        if (m_db->connect()) {
            printSuccess("Connected successfully to MongoDB Database 'StudentManagementDB'.");
        }
    } catch (const DatabaseException& e) {
        printWarning(std::string("MongoDB connection notice: ") + e.what());
        std::cout << "Please ensure MongoDB Community Server is started, or set MONGODB_URI in .env file.\n";
    } catch (const std::exception& e) {
        printError(std::string("Unexpected initialization error: ") + e.what());
    }

    showMainMenu();
}

void StudentManager::showMainMenu() {
    int choice = 0;
    do {
        printHeader("       STUDENT MANAGEMENT SYSTEM       ");
        std::cout << "1. Student Management\n";
        std::cout << "2. Course Management\n";
        std::cout << "3. Attendance Management\n";
        std::cout << "4. Result Management\n";
        std::cout << "5. Search and Filter\n";
        std::cout << "6. Dashboard\n";
        std::cout << "7. Exit\n";
        printDivider();

        choice = getValidInteger("Enter your choice (1-7): ", 1, 7);

        switch (choice) {
            case 1: studentManagementMenu(); break;
            case 2: courseManagementMenu(); break;
            case 3: attendanceManagementMenu(); break;
            case 4: resultManagementMenu(); break;
            case 5: searchAndFilterMenu(); break;
            case 6: showDashboard(); break;
            case 7:
                std::cout << "\nThank you for using the Student Management System. Exiting...\n";
                m_db->disconnect();
                break;
        }
    } while (choice != 7);
}

// ==========================================
// MODULE A: STUDENT MANAGEMENT
// ==========================================

void StudentManager::studentManagementMenu() {
    int choice = 0;
    do {
        printHeader("STUDENT MANAGEMENT MODULE");
        std::cout << "1. Add New Student\n";
        std::cout << "2. Display All Students\n";
        std::cout << "3. Search Student by ID\n";
        std::cout << "4. Search Student by Name\n";
        std::cout << "5. Update Student Information\n";
        std::cout << "6. Delete Student Record\n";
        std::cout << "7. Back to Main Menu\n";
        printDivider();

        choice = getValidInteger("Enter choice (1-7): ", 1, 7);

        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayAllStudents(); break;
            case 3: searchStudentById(); break;
            case 4: searchStudentByName(); break;
            case 5: updateStudent(); break;
            case 6: deleteStudent(); break;
            case 7: break;
        }
    } while (choice != 7);
}

void StudentManager::addStudent() {
    printHeader("ADD NEW STUDENT");
    try {
        std::string id = getValidString("Enter Student ID (e.g. S101): ");
        if (!Student::isValidStudentId(id)) {
            throw ValidationException("Invalid student ID format. Use alphanumeric characters.");
        }
        if (m_db->studentExists(id)) {
            throw DuplicateRecordException("Student with ID '" + id + "' already exists!");
        }

        std::string name = getValidString("Enter Full Name: ");
        int age = getValidInteger("Enter Age (15-100): ", 15, 100);
        std::string gender = getValidString("Enter Gender (Male/Female/Other): ");
        std::string email = getValidString("Enter Email: ");
        if (!Person::isValidEmail(email)) {
            throw ValidationException("Invalid email format (e.g. student@college.edu).");
        }
        std::string phone = getValidString("Enter Phone Number: ");
        if (!Person::isValidPhone(phone)) {
            throw ValidationException("Phone number must contain 7-15 digits.");
        }

        std::string course = getValidString("Enter Course Name (e.g. BCA, B.Tech, MCA): ");
        int sem = getValidInteger("Enter Semester (1-8): ", 1, 8);
        std::string enrollDate = getValidString("Enter Enrollment Date (YYYY-MM-DD): ");

        Student student(id, name, age, email, phone, gender, course, sem, enrollDate);

        if (m_db->insertStudent(student)) {
            printSuccess("Student record saved persistently to MongoDB!");
            displayStudent(student);
        } else {
            printError("Failed to save student record to MongoDB.");
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
    } catch (const std::exception& e) {
        printError(std::string("Unexpected error: ") + e.what());
    }
    pauseConsole();
}

void StudentManager::displayAllStudents() {
    printHeader("ALL REGISTERED STUDENTS");
    try {
        std::vector<Student> students = m_db->getAllStudents();
        displayStudents(students);
    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
    pauseConsole();
}

void StudentManager::searchStudentById() {
    printHeader("SEARCH STUDENT BY ID");
    std::string id = getValidString("Enter Student ID to find: ");
    searchStudent(id);
    pauseConsole();
}

void StudentManager::searchStudentByName() {
    printHeader("SEARCH STUDENT BY NAME");
    std::string name = getValidString("Enter Student Name (or substring) to find: ");
    searchStudent(name, true);
    pauseConsole();
}

void StudentManager::updateStudent() {
    printHeader("UPDATE STUDENT INFORMATION");
    std::string id = getValidString("Enter Student ID to update: ");

    try {
        auto studentOpt = m_db->getStudentById(id);
        if (!studentOpt) {
            throw RecordNotFoundException("Student with ID '" + id + "' not found.");
        }

        Student s = *studentOpt;
        std::cout << "\nCurrent Details:\n";
        displayStudent(s);

        std::cout << "\nLeave field empty to keep current value:\n";

        std::string newName = getValidString("New Full Name [" + s.getName() + "]: ", true);
        if (!newName.empty()) s.setName(newName);

        std::cout << "Update Age? ";
        if (confirmAction("")) {
            int newAge = getValidInteger("New Age (15-100): ", 15, 100);
            s.setAge(newAge);
        }

        std::string newGender = getValidString("New Gender [" + s.getGender() + "]: ", true);
        if (!newGender.empty()) s.setGender(newGender);

        std::string newEmail = getValidString("New Email [" + s.getEmail() + "]: ", true);
        if (!newEmail.empty()) s.setEmail(newEmail);

        std::string newPhone = getValidString("New Phone [" + s.getPhone() + "]: ", true);
        if (!newPhone.empty()) s.setPhone(newPhone);

        std::string newCourse = getValidString("New Course [" + s.getCourse() + "]: ", true);
        if (!newCourse.empty()) s.setCourse(newCourse);

        std::cout << "Update Semester? ";
        if (confirmAction("")) {
            int newSem = getValidInteger("New Semester (1-8): ", 1, 8);
            s.setSemester(newSem);
        }

        if (m_db->updateStudent(s)) {
            printSuccess("Student updated successfully in MongoDB!");
            displayStudent(s);
        } else {
            printWarning("No changes were applied or update failed.");
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
    pauseConsole();
}

void StudentManager::deleteStudent() {
    printHeader("DELETE STUDENT RECORD");
    std::string id = getValidString("Enter Student ID to delete: ");

    try {
        auto studentOpt = m_db->getStudentById(id);
        if (!studentOpt) {
            throw RecordNotFoundException("Student with ID '" + id + "' does not exist.");
        }

        std::cout << "\nRecord to be deleted:\n";
        displayStudent(*studentOpt);

        if (confirmAction("Are you sure you want to permanently delete this student and their associated data?")) {
            if (m_db->deleteStudent(id)) {
                printSuccess("Student record and related records deleted permanently from MongoDB.");
            } else {
                printError("Failed to delete record from MongoDB.");
            }
        } else {
            std::cout << "Delete operation cancelled by user.\n";
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
    pauseConsole();
}

// ==========================================
// MODULE B: COURSE MANAGEMENT
// ==========================================

void StudentManager::courseManagementMenu() {
    int choice = 0;
    do {
        printHeader("COURSE MANAGEMENT MODULE");
        std::cout << "1. Add Course\n";
        std::cout << "2. Display Available Courses\n";
        std::cout << "3. Assign Course to Student\n";
        std::cout << "4. Update Course Information\n";
        std::cout << "5. Back to Main Menu\n";
        printDivider();

        choice = getValidInteger("Enter choice (1-5): ", 1, 5);

        switch (choice) {
            case 1: addCourse(); break;
            case 2: displayAllCourses(); break;
            case 3: assignCourseToStudent(); break;
            case 4: updateCourse(); break;
            case 5: break;
        }
    } while (choice != 5);
}

void StudentManager::addCourse() {
    printHeader("ADD NEW COURSE");
    try {
        std::string id = getValidString("Enter Course ID (e.g. CS101): ");
        if (!Course::isValidCourseId(id)) {
            throw ValidationException("Invalid Course ID format.");
        }
        if (m_db->courseExists(id)) {
            throw DuplicateRecordException("Course with ID '" + id + "' already exists!");
        }

        std::string name = getValidString("Enter Course Name: ");
        std::string dept = getValidString("Enter Department: ");
        int credits = getValidInteger("Enter Credits (1-10): ", 1, 10);
        std::string instructor = getValidString("Enter Instructor Name: ");

        Course course(id, name, dept, credits, instructor);
        if (m_db->insertCourse(course)) {
            printSuccess("Course added successfully to MongoDB!");
            course.displayCourse();
        } else {
            printError("Failed to add course.");
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
    pauseConsole();
}

void StudentManager::displayAllCourses() {
    printHeader("AVAILABLE COURSES");
    try {
        std::vector<Course> courses = m_db->getAllCourses();
        if (courses.empty()) {
            std::cout << "No course records found in MongoDB.\n";
        } else {
            std::cout << "+---------------------------------------------------------------------------------------+\n";
            std::cout << "| Course ID  | Course Name                    | Department       | Credits | Instructor |\n";
            std::cout << "+------------+--------------------------------+------------------+---------+------------+\n";
            for (const auto& c : courses) {
                std::cout << "| " << std::left << std::setw(10) << c.getCourseId()
                          << " | " << std::setw(30) << c.getCourseName()
                          << " | " << std::setw(16) << c.getDepartment()
                          << " | " << std::right << std::setw(7) << c.getCredits()
                          << " | " << std::left << std::setw(10) << c.getInstructor() << " |\n";
            }
            std::cout << "+---------------------------------------------------------------------------------------+\n";
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
    pauseConsole();
}

void StudentManager::assignCourseToStudent() {
    printHeader("ASSIGN COURSE TO STUDENT");
    try {
        std::string sid = getValidString("Enter Student ID: ");
        auto studentOpt = m_db->getStudentById(sid);
        if (!studentOpt) {
            throw RecordNotFoundException("Student with ID '" + sid + "' not found.");
        }

        std::string cid = getValidString("Enter Course ID to assign: ");
        auto courseOpt = m_db->getCourseById(cid);
        if (!courseOpt) {
            throw RecordNotFoundException("Course with ID '" + cid + "' not found.");
        }

        Student s = *studentOpt;
        s.setCourse(courseOpt->getCourseName());
        if (m_db->updateStudent(s)) {
            printSuccess("Student '" + s.getName() + "' assigned to course '" + courseOpt->getCourseName() + "'!");
        } else {
            printError("Failed to assign course.");
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
    pauseConsole();
}

void StudentManager::updateCourse() {
    printHeader("UPDATE COURSE INFORMATION");
    try {
        std::string cid = getValidString("Enter Course ID to update: ");
        auto courseOpt = m_db->getCourseById(cid);
        if (!courseOpt) {
            throw RecordNotFoundException("Course '" + cid + "' does not exist.");
        }

        Course c = *courseOpt;
        std::cout << "\nCurrent Course Details:\n";
        c.displayCourse();

        std::string newName = getValidString("New Name [" + c.getCourseName() + "]: ", true);
        if (!newName.empty()) c.setCourseName(newName);

        std::string newDept = getValidString("New Department [" + c.getDepartment() + "]: ", true);
        if (!newDept.empty()) c.setDepartment(newDept);

        std::cout << "Update credits? ";
        if (confirmAction("")) {
            int newCredits = getValidInteger("New Credits (1-10): ", 1, 10);
            c.setCredits(newCredits);
        }

        std::string newInst = getValidString("New Instructor [" + c.getInstructor() + "]: ", true);
        if (!newInst.empty()) c.setInstructor(newInst);

        if (m_db->updateCourse(c)) {
            printSuccess("Course updated in MongoDB!");
            c.displayCourse();
        } else {
            printWarning("No changes were made.");
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
    pauseConsole();
}

// ==========================================
// MODULE C: ATTENDANCE MANAGEMENT
// ==========================================

void StudentManager::attendanceManagementMenu() {
    int choice = 0;
    do {
        printHeader("ATTENDANCE MANAGEMENT MODULE");
        std::cout << "1. Mark Student Attendance\n";
        std::cout << "2. View Attendance Records by Student\n";
        std::cout << "3. Calculate Attendance Percentage\n";
        std::cout << "4. Back to Main Menu\n";
        printDivider();

        choice = getValidInteger("Enter choice (1-4): ", 1, 4);

        switch (choice) {
            case 1: markAttendance(); break;
            case 2: viewAttendanceRecords(); break;
            case 3: calculateAttendancePercentage(); break;
            case 4: break;
        }
    } while (choice != 4);
}

void StudentManager::markAttendance() {
    printHeader("MARK STUDENT ATTENDANCE");
    try {
        std::string sid = getValidString("Enter Student ID: ");
        if (!m_db->studentExists(sid)) {
            throw RecordNotFoundException("Student '" + sid + "' not found.");
        }

        std::string cid = getValidString("Enter Course ID: ");
        std::string date = getValidString("Enter Date (YYYY-MM-DD): ");
        if (!Attendance::isValidDateFormat(date)) {
            throw ValidationException("Invalid date format. Use YYYY-MM-DD.");
        }

        int statusChoice = getValidInteger("Select Status (1: Present, 2: Absent): ", 1, 2);
        std::string status = (statusChoice == 1) ? "Present" : "Absent";

        std::string remarks = getValidString("Enter Remarks (e.g. Regular, Medical, Late): ", true);
        if (remarks.empty()) remarks = "Regular";

        Attendance att(sid, cid, date, status, remarks);
        if (m_db->recordAttendance(att)) {
            printSuccess("Attendance recorded successfully in MongoDB!");
        } else {
            printError("Failed to record attendance.");
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
    pauseConsole();
}

void StudentManager::viewAttendanceRecords() {
    printHeader("VIEW ATTENDANCE RECORDS");
    try {
        std::string sid = getValidString("Enter Student ID: ");
        std::vector<Attendance> records = m_db->getAttendanceByStudent(sid);
        if (records.empty()) {
            std::cout << "No attendance records found for student '" << sid << "'.\n";
        } else {
            std::cout << "+---------------------------------------------------------------------+\n";
            std::cout << "| Student ID   | Course ID  | Date         | Status   | Remarks          |\n";
            std::cout << "+--------------+------------+--------------+----------+------------------+\n";
            for (const auto& a : records) {
                a.displayAttendance();
            }
            std::cout << "+---------------------------------------------------------------------+\n";
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
    pauseConsole();
}

void StudentManager::calculateAttendancePercentage() {
    printHeader("CALCULATE ATTENDANCE PERCENTAGE");
    try {
        std::string sid = getValidString("Enter Student ID: ");
        auto studentOpt = m_db->getStudentById(sid);
        if (!studentOpt) {
            throw RecordNotFoundException("Student '" + sid + "' not found.");
        }

        auto [present, total] = m_db->getAttendanceStats(sid);
        std::cout << "\nAttendance Summary for " << studentOpt->getName() << " (ID: " << sid << "):\n";
        std::cout << "Total Classes Conducted: " << total << "\n";
        std::cout << "Total Classes Attended : " << present << "\n";

        if (total > 0) {
            double pct = (static_cast<double>(present) / total) * 100.0;
            std::cout << "Attendance Percentage  : " << std::fixed << std::setprecision(2) << pct << "%\n";
            if (pct >= 75.0) {
                printSuccess("Attendance criteria met (>= 75%). Eligible for examinations.");
            } else {
                printWarning("Attendance is below 75%! Student is in the shortage list.");
            }
        } else {
            std::cout << "No attendance records entered yet.\n";
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
    pauseConsole();
}

// ==========================================
// MODULE D: RESULT MANAGEMENT
// ==========================================

void StudentManager::resultManagementMenu() {
    int choice = 0;
    do {
        printHeader("RESULT MANAGEMENT MODULE");
        std::cout << "1. Add / Update Student Marks\n";
        std::cout << "2. Display Student Result Card\n";
        std::cout << "3. Display All Student Rankings\n";
        std::cout << "4. Back to Main Menu\n";
        printDivider();

        choice = getValidInteger("Enter choice (1-4): ", 1, 4);

        switch (choice) {
            case 1: addStudentMarks(); break;
            case 2: displayStudentResult(); break;
            case 3: displayAllResultRankings(); break;
            case 4: break;
        }
    } while (choice != 4);
}

void StudentManager::addStudentMarks() {
    printHeader("ADD STUDENT EXAMINATION MARKS");
    try {
        std::string sid = getValidString("Enter Student ID: ");
        auto studentOpt = m_db->getStudentById(sid);
        if (!studentOpt) {
            throw RecordNotFoundException("Student '" + sid + "' does not exist.");
        }

        std::string cid = getValidString("Enter Course / Examination Code: ");
        int numSubjects = getValidInteger("Enter number of subjects to record (1-10): ", 1, 10);

        Result result(sid, cid);

        for (int i = 1; i <= numSubjects; ++i) {
            std::cout << "\nSubject #" << i << ":\n";
            std::string subName = getValidString("  Subject Name: ");
            double marks = getValidDouble("  Marks Obtained (0-100): ", 0.0, 100.0);
            double maxMarks = getValidDouble("  Max Marks (usually 100): ", 1.0, 200.0);
            result.addSubjectMark(subName, marks, maxMarks);
        }

        if (m_db->recordResult(result)) {
            printSuccess("Examination result saved to MongoDB!");
            result.displayResult();
        } else {
            printError("Failed to save result.");
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
    pauseConsole();
}

void StudentManager::displayStudentResult() {
    printHeader("DISPLAY STUDENT RESULT");
    try {
        std::string sid = getValidString("Enter Student ID: ");
        auto resOpt = m_db->getResultByStudent(sid);
        if (resOpt) {
            resOpt->displayResult();
        } else {
            std::cout << "No examination results found for student '" << sid << "'.\n";
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
    pauseConsole();
}

void StudentManager::displayAllResultRankings() {
    printHeader("ALL STUDENT EXAMINATION RANKINGS");
    try {
        std::vector<Result> results = m_db->getAllResults();
        if (results.empty()) {
            std::cout << "No result records found in MongoDB.\n";
        } else {
            // Sort by percentage descending
            std::sort(results.begin(), results.end(), [](const Result& a, const Result& b) {
                return a.getPercentage() > b.getPercentage();
            });

            std::cout << "+-----------------------------------------------------------------------------+\n";
            std::cout << "| Rank | Student ID   | Course ID  | Total Marks | Max Marks | Percentage | Grade |\n";
            std::cout << "+------+--------------+------------+-------------+-----------+------------+-------+\n";
            int rank = 1;
            for (const auto& r : results) {
                std::cout << "| " << std::setw(4) << rank++
                          << " | " << std::left << std::setw(12) << r.getStudentId()
                          << " | " << std::setw(10) << r.getCourseId()
                          << " | " << std::right << std::setw(11) << static_cast<int>(r.getTotalMarks())
                          << " | " << std::setw(9) << static_cast<int>(r.getMaxPossibleMarks())
                          << " | " << std::setw(9) << std::fixed << std::setprecision(2) << r.getPercentage() << "%"
                          << " | " << std::setw(5) << r.getGrade() << " |\n";
            }
            std::cout << "+-----------------------------------------------------------------------------+\n";
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
    pauseConsole();
}

// ==========================================
// MODULE E: SEARCH AND FILTER
// ==========================================

void StudentManager::searchAndFilterMenu() {
    int choice = 0;
    do {
        printHeader("SEARCH AND FILTER MODULE");
        std::cout << "1. Search Student by ID\n";
        std::cout << "2. Search Student by Name\n";
        std::cout << "3. Filter Students by Course\n";
        std::cout << "4. Sort Students (Name / Marks)\n";
        std::cout << "5. Back to Main Menu\n";
        printDivider();

        choice = getValidInteger("Enter choice (1-5): ", 1, 5);

        switch (choice) {
            case 1: searchStudentById(); break;
            case 2: searchStudentByName(); break;
            case 3: filterStudentsByCourse(); break;
            case 4: sortStudentsMenu(); break;
            case 5: break;
        }
    } while (choice != 5);
}

void StudentManager::filterStudentsByCourse() {
    printHeader("FILTER STUDENTS BY COURSE");
    std::string course = getValidString("Enter Course Name to filter (e.g. BCA): ");
    try {
        std::vector<Student> students = m_db->getStudentsByCourse(course);
        std::cout << "\nFound " << students.size() << " student(s) enrolled in '" << course << "':\n";
        displayStudents(students);
    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
    pauseConsole();
}

void StudentManager::sortStudentsMenu() {
    printHeader("SORT STUDENTS");
    std::cout << "1. Sort by Name (Ascending: A to Z)\n";
    std::cout << "2. Sort by Name (Descending: Z to A)\n";
    std::cout << "3. Sort by Result Percentage (Highest to Lowest)\n";
    std::cout << "4. Sort by Result Percentage (Lowest to Highest)\n";
    printDivider();

    int choice = getValidInteger("Choose sorting criterion (1-4): ", 1, 4);
    switch (choice) {
        case 1: sortStudents(SortCriteria::ByNameAscending); break;
        case 2: sortStudents(SortCriteria::ByNameDescending); break;
        case 3: sortStudents(SortCriteria::ByPercentageDescending); break;
        case 4: sortStudents(SortCriteria::ByPercentageAscending); break;
    }
    pauseConsole();
}

void StudentManager::sortStudents(SortCriteria criteria) {
    try {
        std::vector<Student> students = m_db->getAllStudents();
        if (students.empty()) {
            std::cout << "No student records available to sort.\n";
            return;
        }

        if (criteria == SortCriteria::ByNameAscending) {
            std::sort(students.begin(), students.end(), [](const Student& a, const Student& b) {
                return a.getName() < b.getName();
            });
            std::cout << "\nSorted by Name (A -> Z):\n";
            displayStudents(students);
        } else if (criteria == SortCriteria::ByNameDescending) {
            std::sort(students.begin(), students.end(), [](const Student& a, const Student& b) {
                return a.getName() > b.getName();
            });
            std::cout << "\nSorted by Name (Z -> A):\n";
            displayStudents(students);
        } else {
            // Sort by percentage: Map student IDs to percentage
            std::map<std::string, double> percentageMap;
            std::vector<Result> results = m_db->getAllResults();
            for (const auto& r : results) {
                percentageMap[r.getStudentId()] = r.getPercentage();
            }

            if (criteria == SortCriteria::ByPercentageDescending) {
                std::sort(students.begin(), students.end(), [&](const Student& a, const Student& b) {
                    double pA = percentageMap.count(a.getId()) ? percentageMap[a.getId()] : -1.0;
                    double pB = percentageMap.count(b.getId()) ? percentageMap[b.getId()] : -1.0;
                    return pA > pB;
                });
                std::cout << "\nSorted by Examination Percentage (Highest -> Lowest):\n";
                displayStudents(students);
            } else {
                std::sort(students.begin(), students.end(), [&](const Student& a, const Student& b) {
                    double pA = percentageMap.count(a.getId()) ? percentageMap[a.getId()] : 101.0;
                    double pB = percentageMap.count(b.getId()) ? percentageMap[b.getId()] : 101.0;
                    return pA < pB;
                });
                std::cout << "\nSorted by Examination Percentage (Lowest -> Highest):\n";
                displayStudents(students);
            }
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
}

// ==========================================
// MODULE F: DASHBOARD
// ==========================================

void StudentManager::showDashboard() {
    printHeader("COLLEGE DASHBOARD OVERVIEW");
    try {
        DashboardStats stats = m_db->getDashboardStats();

        std::cout << "+-------------------------------------------------------------+\n";
        std::cout << "|                      KEY METRICS                            |\n";
        std::cout << "+-------------------------------------------------------------+\n";
        std::cout << "| Total Registered Students     : " << std::left << std::setw(28) << stats.totalStudents << "|\n";
        std::cout << "| Total Active Courses          : " << std::left << std::setw(28) << stats.totalCourses << "|\n";
        std::cout << "| Total Attendance Records Logged: " << std::left << std::setw(27) << stats.totalAttendanceRecords << "|\n";
        
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2) << stats.averagePercentage << "%";
        std::cout << "| Overall Student Average Mark  : " << std::left << std::setw(28) << ss.str() << "|\n";
        std::cout << "+-------------------------------------------------------------+\n";

        // Display Active In-Memory Student Instances (Static OOP Demo)
        std::cout << "\n* OOP Static Metric: " << Student::getActiveStudentCount() 
                  << " Student object instance(s) currently held in memory.\n";

    } catch (const StudentManagementException& e) {
        printError(e.what());
    }
    pauseConsole();
}

// ==========================================
// FUNCTION OVERLOADING & POLYMORPHISM DEMO
// ==========================================

// Overloaded display for a single student (Demonstrates Runtime Polymorphism)
void StudentManager::displayStudent(const Student& student) const {
    // Polymorphism: Pointing to Student via Person* base pointer
    const Person* personPtr = &student;
    // Dispatches polymorphically to Student::displayInfo()
    personPtr->displayInfo();
}

// Overloaded display for a vector of students
void StudentManager::displayStudents(const std::vector<Student>& students) const {
    if (students.empty()) {
        std::cout << "No student records found.\n";
        return;
    }

    std::cout << "+-----------------------------------------------------------------------------------------------------------+\n";
    std::cout << "| ID         | Name                 | Age | Gender | Course          | Sem | Email                     | Phone  |\n";
    std::cout << "+------------+----------------------+-----+--------+-----------------+-----+---------------------------+--------+\n";
    for (const auto& s : students) {
        std::cout << "| " << std::left << std::setw(10) << s.getId()
                  << " | " << std::setw(20) << s.getName()
                  << " | " << std::right << std::setw(3) << s.getAge()
                  << " | " << std::left << std::setw(6) << s.getGender()
                  << " | " << std::setw(15) << s.getCourse()
                  << " | " << std::right << std::setw(3) << s.getSemester()
                  << " | " << std::left << std::setw(25) << s.getEmail()
                  << " | " << std::setw(6) << s.getPhone() << " |\n";
    }
    std::cout << "+-----------------------------------------------------------------------------------------------------------+\n";
    std::cout << "Total Records: " << students.size() << "\n";
}

// Overloaded search by ID
bool StudentManager::searchStudent(const std::string& studentId) {
    try {
        auto studentOpt = m_db->getStudentById(studentId);
        if (studentOpt) {
            printSuccess("Student record found!");
            displayStudent(*studentOpt);
            return true;
        } else {
            std::cout << "No student found with ID: " << studentId << "\n";
            return false;
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
        return false;
    }
}

// Overloaded search by Name
bool StudentManager::searchStudent(const std::string& name, bool searchByName) {
    if (!searchByName) return searchStudent(name);
    try {
        std::vector<Student> matches = m_db->getStudentsByName(name);
        if (!matches.empty()) {
            printSuccess("Matching student records found: " + std::to_string(matches.size()));
            displayStudents(matches);
            return true;
        } else {
            std::cout << "No students found matching name query: '" << name << "'\n";
            return false;
        }
    } catch (const StudentManagementException& e) {
        printError(e.what());
        return false;
    }
}
