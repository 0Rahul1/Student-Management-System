#ifndef COURSE_H
#define COURSE_H

#include <string>

/**
 * @brief Represents an Academic Course.
 * Demonstrates Encapsulation with validated fields.
 */
class Course {
private:
    std::string m_courseId;
    std::string m_courseName;
    std::string m_department;
    int m_credits;
    std::string m_instructor;

public:
    // Default constructor
    Course();

    // Parameterized constructor
    Course(std::string id, std::string name, std::string dept, 
           int credits, std::string instructor);

    // Getters
    std::string getCourseId() const;
    std::string getCourseName() const;
    std::string getDepartment() const;
    int getCredits() const;
    std::string getInstructor() const;

    // Setters with validation
    void setCourseId(const std::string& id);
    void setCourseName(const std::string& name);
    void setDepartment(const std::string& dept);
    void setCredits(int credits);
    void setInstructor(const std::string& instructor);

    // Display course info
    void displayCourse() const;

    // Validation helper
    static bool isValidCourseId(const std::string& id);
};

#endif // COURSE_H
