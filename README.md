# Student Management System (C++ & MongoDB)

A professional, robust, Object-Oriented Programming (OOP) college project engineered in modern **C++ (C++17)** with persistent storage powered by **MongoDB**.

---

## 1. Project Introduction

The **Student Management System** is an enterprise-grade console application built for academic institutions (colleges, universities, and training institutes). It automates and streamlines student enrollment, course allocation, daily attendance tracking, exam score management, report card generation, and real-time dashboard analytics.

Unlike basic academic projects that use volatile in-memory arrays or flat text/JSON files, this project leverages the **official MongoDB database architecture** using BSON documents, collections, and structured indexing to ensure high data integrity, persistence, and scalability.

---

## 2. Problem Statement

Traditional paper-based or basic file-based student tracking systems suffer from:
- **Data Redundancy and Loss**: Flat files are prone to corruption, concurrent write conflicts, and accidental deletion.
- **Lack of Integrity**: Without unique constraints, duplicate student roll numbers and contradictory enrollments occur.
- **Complex Querying**: Searching, filtering by course, and calculating average GPA/percentages from raw text files is inefficient and error-prone.
- **Disconnected Records**: Attendance and academic scores are often fragmented across disparate files without relationship consistency.

---

## 3. Project Objectives

1. **Practical OOP Demonstration**: Provide an exemplar demonstration of all 12 core Object-Oriented Programming principles in clean, modern C++.
2. **Persistent Enterprise Storage**: Integrate directly with MongoDB (`StudentManagementDB`) supporting both local MongoDB Community Server and cloud-hosted MongoDB Atlas.
3. **Comprehensive Academic Management**: Deliver end-to-end management of Students, Courses, Attendance logs, and Examination results.
4. **Resilient Error Handling**: Enforce domain validation rules (prevent duplicate IDs, ensure score bounds, email regex checks) with custom C++ exception types.
5. **Intuitive Console UI**: Implement a formatted, menu-driven CLI interface complete with input validation and summary tables.

---

## 4. Technologies Used

| Technology / Tool | Version / Standard | Purpose |
|---|---|---|
| **Programming Language** | C++17 | Core backend application logic & OOP architecture |
| **Database** | MongoDB 6.x / 7.x / 8.x / 9.x | Persistent document-oriented NoSQL database |
| **Database Driver** | MongoDB C++ Driver (`mongocxx` & `bsoncxx`) | Official MongoDB driver interfacing C++ with the database engine |
| **Build System** | CMake 3.15+ | Cross-platform build automation and dependency linking |
| **Compiler** | MinGW-w64 GCC (or MSVC) | Compiles modern C++ code on Windows |
| **IDE** | Visual Studio Code | Development, debugging, and terminal execution |
| **Operating System** | Windows 10 / 11 | Deployment and test environment |

---

## 5. OOP Concepts Implemented

This project comprehensively incorporates the following 12 Object-Oriented Programming concepts:

### 1. Classes and Objects
- Defined in distinct modular headers: `Person`, `Student`, `Course`, `Attendance`, `Result`, `Database`, and `StudentManager`.
- Real objects are instantiated to model real-world institutional entities.

### 2. Encapsulation
- Member variables are declared `private` or `protected`.
- State access is mediated strictly through `public` getters and setters with business validation (e.g., student age must be 15–100, examination marks must not exceed maximum marks, semester must be 1–8).

### 3. Abstraction
- The `Person` class acts as an **Abstract Base Class (ABC)** defining the interface method:
  ```cpp
  virtual void displayInfo() const = 0;
  ```
- The `Database` class completely abstracts low-level MongoDB wire protocol commands, BSON encoders, and connection pools behind simple domain functions (`insertStudent`, `getAllStudents`, etc.).
- Implemented with the **PIMPL (Pointer to Implementation)** idiom to insulate clients from internal driver structures.

### 4. Inheritance
- `Student` inherits directly from `Person`:
  ```cpp
  class Student : public Person { ... };
  ```
- Reuses common identity properties (`id`, `name`, `age`, `email`, `phone`, `gender`) while extending with academic fields (`course`, `semester`, `enrollmentDate`).

### 5. Polymorphism
- **Runtime Polymorphism**: Virtual dispatch via base class pointer `const Person* personPtr = &student; personPtr->displayInfo();` invokes `Student::displayInfo()` dynamically at runtime.
- **Compile-time Polymorphism**: Function overloading for display routines, searches, and sort operations.

### 6. Constructors and Destructors
- Default constructors, parameterized constructors with initialization lists, copy constructors, and virtual destructors (`virtual ~Person()`, `~Student()`, `~Database()`) ensuring clean memory deallocation without resource leaks.

### 7. Function Overloading
- Overloaded methods in `StudentManager`:
  - `void displayStudent(const Student& s) const;`
  - `void displayStudents(const std::vector<Student>& list) const;`
  - `bool searchStudent(const std::string& id);`
  - `bool searchStudent(const std::string& name, bool searchByName);`

### 8. Function Overriding
- `Student::displayInfo() const override;` provides a formatted student profile card, overriding the pure virtual `Person::displayInfo()`.

### 9. Access Specifiers
- `private`: Sensitive internal attributes (`m_course`, `m_semester`, `m_impl`).
- `protected`: Inherited member variables in `Person` (`m_id`, `m_name`, etc.) accessible to the derived `Student` class.
- `public`: Clean, documented public API methods.

### 10. Static Members
- `Student::s_activeStudentCount`: Tracks active student objects held in memory.
- `Student::getActiveStudentCount()`: Static accessor for memory audits.
- `Result::calculateGrade(double percentage)`: Static calculation utility converting percentages into academic letter grades (A+, A, B, C, D, F).
- `Person::isValidEmail(const std::string&)`: Static regex validation helper.

### 11. Exception Handling
- Custom exception hierarchy derived from `std::exception`:
  - `StudentManagementException` (Base)
  - `DatabaseException` (MongoDB connection/query failure)
  - `ValidationException` (Invalid age, email, marks out of bounds)
  - `RecordNotFoundException` (Non-existent ID query)
  - `DuplicateRecordException` (Duplicate Student ID collision)
- All critical flows are protected with `try-catch` blocks providing friendly error messages without application crashes.

### 12. Composition
- `StudentManager` **HAS-A** `Database` instance (`std::unique_ptr<Database> m_db`).
- `Result` **HAS-A** collection of subject score records (`std::vector<SubjectEntry> m_subjects`).

---

## 6. System Architecture

```
+-------------------------------------------------------------------------+
|                              USER INTERFACE                             |
|               (Console Menu-Driven Terminal System)                     |
+-------------------------------------------------------------------------+
                                    |
                                    v
+-------------------------------------------------------------------------+
|                         STUDENT MANAGER CONTROLLER                      |
|  - Student Management        - Course Management                        |
|  - Attendance Management     - Result Management                        |
|  - Search & Filter Engine    - Analytics Dashboard                      |
+-------------------------------------------------------------------------+
        |                    |                     |              |
        v                    v                     v              v
+---------------+    +---------------+    +---------------+  +------------+
|    Student    |    |     Course    |    |   Attendance  |  |   Result   |
| (is-a Person) |    +---------------+    +---------------+  +------------+
+---------------+
        |
        +-----------------------------------+
                                            |
                                            v
+-------------------------------------------------------------------------+
|                         DATABASE LAYER (Database.cpp)                   |
|    - PIMPL Encapsulation         - Indexing Engine                      |
|    - Connection Manager (.env)   - MongoDB CRUD / Aggregation Pipeline  |
+-------------------------------------------------------------------------+
                                    |
                                    v
+-------------------------------------------------------------------------+
|                  OFFICIAL MONGODB C++ DRIVER (mongocxx)                 |
+-------------------------------------------------------------------------+
                                    |
                                    v
+-------------------------------------------------------------------------+
|                 MONGODB SERVER (StudentManagementDB)                    |
|   Collections: [students]  [courses]  [attendance]  [results]           |
+-------------------------------------------------------------------------+
```

---

## 7. Database Design

- **Database Name**: `StudentManagementDB`

### Collection 1: `students`
Stores individual student profiles with a unique index on `student_id`.
```json
{
  "_id": ObjectId("..."),
  "student_id": "S101",
  "name": "Aarav Sharma",
  "age": 20,
  "gender": "Male",
  "email": "aarav.sharma@college.edu",
  "phone": "+91-9876543210",
  "course": "Bachelor of Computer Applications",
  "semester": 3,
  "enrollment_date": "2026-08-01"
}
```

### Collection 2: `courses`
Stores curriculum courses with a unique index on `course_id`.
```json
{
  "_id": ObjectId("..."),
  "course_id": "BCA301",
  "course_name": "Data Structures & OOP in C++",
  "department": "Computer Science",
  "credits": 4,
  "instructor": "Dr. Sneha Roy"
}
```

### Collection 3: `attendance`
Stores daily attendance records with an index on `(student_id, date)`.
```json
{
  "_id": ObjectId("..."),
  "student_id": "S101",
  "course_id": "BCA301",
  "date": "2026-10-30",
  "status": "Present",
  "remarks": "Regular Lecture"
}
```

### Collection 4: `results`
Stores comprehensive semester results and subject breakdowns.
```json
{
  "_id": ObjectId("..."),
  "student_id": "S101",
  "course_id": "BCA301",
  "total_marks": 275.0,
  "max_marks": 300.0,
  "percentage": 91.67,
  "grade": "A+",
  "subjects": [
    { "subject_name": "Object-Oriented Programming", "marks_obtained": 95.0, "max_marks": 100.0 },
    { "subject_name": "Database Systems", "marks_obtained": 90.0, "max_marks": 100.0 },
    { "subject_name": "Computer Architecture", "marks_obtained": 90.0, "max_marks": 100.0 }
  ]
}
```

---

## 8. Application Modules & Features

### Module A: Student Management
- **Add Student**: Prompts for all student data, verifies formatting, prevents duplicate IDs.
- **Display All**: Displays complete tabular overview of registered students.
- **Search by ID / Name**: Instant exact match and regex-based substring searches.
- **Update Student**: Interactive prompt allowing partial updates while keeping unchanged fields.
- **Delete Record**: Prompts user confirmation (`y/n`) and cascade-deletes related attendance & result documents.

### Module B: Course Management
- **Add Course**: Inserts course record with code, credits, department, and faculty.
- **Display Available Courses**: Tabular display with credits and instructors.
- **Assign Course to Student**: Dynamically links students to registered course codes.
- **Update Course**: Modify course title, instructor, or credit load.

### Module C: Attendance Management
- **Mark Attendance**: Logs date-stamped attendance status (`Present` / `Absent`) with remarks.
- **View Attendance Records**: Displays chronological history for any given student ID.
- **Calculate Percentage**: Computes overall attendance percentage and evaluates against the mandatory **75% minimum attendance rule** with advisory warnings.

### Module D: Result Management
- **Add Marks**: Dynamic entry for 1 to 10 subjects per student with automatic validation.
- **Compute Totals & Percentages**: Calculates aggregated score, percentage, and grade:
  - **90%+**: `A+`
  - **80% - 89.9%**: `A`
  - **70% - 79.9%**: `B`
  - **60% - 69.9%**: `C`
  - **50% - 59.9%**: `D`
  - **< 50%**: `F` (Fail)
- **Student Report Card**: Formatted academic performance transcript.
- **All Student Rankings**: Auto-sorted merit list based on examination percentages.

### Module E: Search and Filter
- Fast lookups by Student ID.
- Case-insensitive search by student name.
- Filter student cohort by enrolled course.
- Sort students alphabetically (A–Z / Z–A) or by examination percentage.

### Module F: Real-time Dashboard
- Aggregates and displays:
  - Total student enrollment count.
  - Total active courses count.
  - Total attendance logs recorded.
  - Overall college-wide student examination average percentage (using MongoDB aggregation pipeline `$avg`).
  - Active in-memory C++ static object audit.

---

## 9. Windows Installation & Setup Guide

### Step 1: Install Visual Studio Code
Download and install Visual Studio Code from:
[https://code.visualstudio.com/](https://code.visualstudio.com/)

Install the following VS Code extensions:
- **C/C++** (by Microsoft)
- **CMake Tools** (by Microsoft)
- **MongoDB for VS Code** (by MongoDB)

### Step 2: Install C++ Compiler & CMake
If you don't already have MinGW and CMake, install via PowerShell:
```powershell
winget install --id Kitware.CMake -e
winget install --id BrechtSanders.WinLibs.POSIX.UCRT -e
```
Verify the installation:
```powershell
cmake --version
g++ --version
```

### Step 3: Install MongoDB Community Server
Install MongoDB Server on your Windows machine:
```powershell
winget install --id MongoDB.Server -e
```
Verify MongoDB is running:
```powershell
Get-Service -Name "*mongo*"
```
If not started, start the MongoDB service:
```powershell
net start MongoDB
```

> **Alternative: Use MongoDB Atlas (Cloud Free Tier)**
> If you prefer not to run MongoDB locally, create a free cluster at [https://www.mongodb.com/cloud/atlas](https://www.mongodb.com/cloud/atlas) and copy your connection string.

### Step 4: Configure Database Credentials
Copy `.env.example` to `.env`:
```powershell
Copy-Item .env.example .env
```
Edit `.env` to configure your MongoDB connection:
```ini
# For Local MongoDB
MONGODB_URI=mongodb://localhost:27017

# OR For MongoDB Atlas Cloud
# MONGODB_URI=mongodb+srv://admin:YourPassword123@cluster0.mongodb.net/?retryWrites=true&w=majority

MONGODB_DATABASE=StudentManagementDB
```

### Step 5: Install MongoDB C++ Driver (mongocxx)
To compile with the official `mongocxx` driver using vcpkg:
```powershell
# In PowerShell:
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
C:\vcpkg\bootstrap-vcpkg.bat
C:\vcpkg\vcpkg.exe install mongo-cxx-driver:x64-windows
```

---

## 10. Compilation and Execution

### Build Using CMake
Open PowerShell in the project directory:
```powershell
# 1. Create build directory and configure CMake
cmake -B build -S . -G "MinGW Makefiles"

# If building with vcpkg MongoDB driver:
# cmake -B build -S . -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake

# 2. Compile the project
cmake --build build

# 3. Run the application
.\build\StudentManagementSystem.exe
```

---

## 11. Sample Input and Output

### Adding a Student
```text
=============================================================
  ADD NEW STUDENT
=============================================================
Enter Student ID (e.g. S101): S101
Enter Full Name: Sneha Sharma
Enter Age (15-100): 20
Enter Gender (Male/Female/Other): Female
Enter Email: sneha.sharma@college.edu
Enter Phone Number: +91-9876543210
Enter Course Name (e.g. BCA, B.Tech, MCA): BCA
Enter Semester (1-8): 4
Enter Enrollment Date (YYYY-MM-DD): 2026-08-15

[SUCCESS] Student record saved persistently to MongoDB!

+-------------------------------------------------------------+
|                      STUDENT PROFILE                        |
+-------------------------------------------------------------+
| ID             : S101                                       |
| Full Name      : Sneha Sharma                               |
| Age            : 20                                         |
| Gender         : Female                                     |
| Email          : sneha.sharma@college.edu                   |
| Phone          : +91-9876543210                             |
| Course         : BCA                                        |
| Semester       : 4                                          |
| Enrollment Date: 2026-08-15                                 |
+-------------------------------------------------------------+
```

### Viewing Student Report Card
```text
+-------------------------------------------------------------+
|                     STUDENT REPORT CARD                     |
+-------------------------------------------------------------+
| Student ID : S101                                           |
| Course ID  : BCA301                                         |
+-------------------------------------------------------------+
| Subject                    | Marks Obtained | Max Marks     |
+----------------------------+----------------+---------------+
| Object-Oriented C++        |           94.0 |           100 |
| Database Management        |           88.5 |           100 |
| Data Structures            |           92.0 |           100 |
+----------------------------+----------------+---------------+
| Total Marks: 274 / 300     | Percentage:  91.33% | Grade: A+  |
+-------------------------------------------------------------+
```

---

## 12. Future Enhancements

1. **Role-Based Authentication**: Admin, Faculty, and Student login portals with hashed passwords (bcrypt/Argon2).
2. **Graphical User Interface (GUI)**: Modern Qt 6 or Dear ImGui desktop interface.
3. **Automated PDF Export**: Generation of printable PDF transcripts and grade sheets.
4. **Biometric & RFID Attendance**: Hardware integration with student ID card scanners.

---

## 13. Conclusion

The **Student Management System** bridges the gap between academic Object-Oriented Programming theory and real-world database software development. By marrying clean C++ design patterns with MongoDB's document persistence, the project delivers a reliable, scalable, and interview-ready software engineering solution.
