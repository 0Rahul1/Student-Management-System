#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>
#include <string>

/**
 * @brief Base exception class for Student Management System.
 * Demonstrates OOP Exception Handling and Polymorphic Exceptions.
 */
class StudentManagementException : public std::exception {
protected:
    std::string m_message;
public:
    explicit StudentManagementException(std::string message) 
        : m_message(std::move(message)) {}

    const char* what() const noexcept override {
        return m_message.c_str();
    }
};

/**
 * @brief Exception thrown on MongoDB connection or query errors.
 */
class DatabaseException : public StudentManagementException {
public:
    explicit DatabaseException(const std::string& message)
        : StudentManagementException("[Database Error] " + message) {}
};

/**
 * @brief Exception thrown when input data validation fails (e.g. invalid age, bad email).
 */
class ValidationException : public StudentManagementException {
public:
    explicit ValidationException(const std::string& message)
        : StudentManagementException("[Validation Error] " + message) {}
};

/**
 * @brief Exception thrown when an expected record is not found in MongoDB.
 */
class RecordNotFoundException : public StudentManagementException {
public:
    explicit RecordNotFoundException(const std::string& message)
        : StudentManagementException("[Record Not Found] " + message) {}
};

/**
 * @brief Exception thrown when attempting to insert a duplicate key (e.g. existing Student ID).
 */
class DuplicateRecordException : public StudentManagementException {
public:
    explicit DuplicateRecordException(const std::string& message)
        : StudentManagementException("[Duplicate Record] " + message) {}
};

#endif // EXCEPTIONS_H
