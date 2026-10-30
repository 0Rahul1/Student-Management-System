#ifndef PERSON_H
#define PERSON_H

#include <string>
#include <iostream>

/**
 * @brief Abstract Base Class representing a Person.
 * 
 * Demonstrates:
 * - Abstraction: Contains pure virtual function displayInfo()
 * - Encapsulation: Protected member variables with public getters/setters & validation
 * - Access Specifiers: protected variables accessible to derived classes, public methods
 * - Constructors and Destructor: Virtual destructor ensures proper cleanup of derived objects
 */
class Person {
protected:
    std::string m_id;
    std::string m_name;
    int m_age;
    std::string m_email;
    std::string m_phone;
    std::string m_gender;

public:
    // Default constructor
    Person();

    // Parameterized constructor
    Person(std::string id, std::string name, int age, 
           std::string email, std::string phone, std::string gender);

    // Virtual destructor (crucial for polymorphic cleanup)
    virtual ~Person();

    // Pure virtual function for Abstraction and Runtime Polymorphism
    virtual void displayInfo() const = 0;

    // Getters
    std::string getId() const;
    std::string getName() const;
    int getAge() const;
    std::string getEmail() const;
    std::string getPhone() const;
    std::string getGender() const;

    // Setters with Encapsulation & Validation
    void setId(const std::string& id);
    void setName(const std::string& name);
    void setAge(int age);
    void setEmail(const std::string& email);
    void setPhone(const std::string& phone);
    void setGender(const std::string& gender);

    // Static helper for email validation
    static bool isValidEmail(const std::string& email);
    // Static helper for phone validation
    static bool isValidPhone(const std::string& phone);
};

#endif // PERSON_H
