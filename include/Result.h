#ifndef RESULT_H
#define RESULT_H

#include <string>
#include <vector>
#include <utility>

/**
 * @brief Represents academic examination results for a student.
 * 
 * Demonstrates:
 * - Composition: Contains collection of subject mark pairs
 * - Static Functions: static calculateGrade(double percentage)
 * - Encapsulation: Ensures calculations match internal state
 */
class Result {
private:
    std::string m_studentId;
    std::string m_courseId;
    // Composition: map/vector of (SubjectName, Score, MaxMarks)
    struct SubjectEntry {
        std::string subjectName;
        double marksObtained;
        double maxMarks;
    };
    std::vector<SubjectEntry> m_subjects;

    double m_totalMarks;
    double m_maxPossibleMarks;
    double m_percentage;
    std::string m_grade;

public:
    // Default constructor
    Result();

    // Parameterized constructor
    Result(std::string studentId, std::string courseId);

    // Getters
    std::string getStudentId() const;
    std::string getCourseId() const;
    double getTotalMarks() const;
    double getMaxPossibleMarks() const;
    double getPercentage() const;
    std::string getGrade() const;
    const std::vector<SubjectEntry>& getSubjects() const;

    // Setters
    void setStudentId(const std::string& studentId);
    void setCourseId(const std::string& courseId);

    // Add individual subject mark
    void addSubjectMark(const std::string& subjectName, double marksObtained, double maxMarks = 100.0);

    // Recalculate totals, percentage, and grade
    void computeTotals();

    // Static member function to determine letter grade
    static std::string calculateGrade(double percentage);

    // Display formatted result card
    void displayResult() const;
};

#endif // RESULT_H
