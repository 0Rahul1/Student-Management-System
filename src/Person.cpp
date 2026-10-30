#include "Person.h"
#include "Exceptions.h"

#include <regex>
#include <utility>

// Default constructor
Person::Person()
    : m_id(""), m_name(""), m_age(18), m_email(""), m_phone(""), m_gender("Unspecified") {
}

// Parameterized constructor
Person::Person(std::string id, std::string name, int age, 
               std::string email, std::string phone, std::string gender)
    : m_id(std::move(id)), m_name(std::move(name)), m_age(age), 
      m_email(std::move(email)), m_phone(std::move(phone)), m_gender(std::move(gender)) {
    
    if (m_id.empty()) {
        throw ValidationException("Person ID cannot be empty.");
    }
    if (m_name.empty()) {
        throw ValidationException("Person Name cannot be empty.");
    }
    if (m_age < 15 || m_age > 100) {
        throw ValidationException("Age must be between 15 and 100.");
    }
    if (!m_email.empty() && !isValidEmail(m_email)) {
        throw ValidationException("Invalid email format: " + m_email);
    }
    if (!m_phone.empty() && !isValidPhone(m_phone)) {
        throw ValidationException("Invalid phone number format: " + m_phone);
    }
}

// Virtual destructor implementation
Person::~Person() = default;

// Getters
std::string Person::getId() const { return m_id; }
std::string Person::getName() const { return m_name; }
int Person::getAge() const { return m_age; }
std::string Person::getEmail() const { return m_email; }
std::string Person::getPhone() const { return m_phone; }
std::string Person::getGender() const { return m_gender; }

// Setters with Encapsulation & Validation
void Person::setId(const std::string& id) {
    if (id.empty()) {
        throw ValidationException("ID cannot be empty.");
    }
    m_id = id;
}

void Person::setName(const std::string& name) {
    if (name.empty()) {
        throw ValidationException("Name cannot be empty.");
    }
    m_name = name;
}

void Person::setAge(int age) {
    if (age < 15 || age > 100) {
        throw ValidationException("Age must be between 15 and 100.");
    }
    m_age = age;
}

void Person::setEmail(const std::string& email) {
    if (!isValidEmail(email)) {
        throw ValidationException("Invalid email address format.");
    }
    m_email = email;
}

void Person::setPhone(const std::string& phone) {
    if (!isValidPhone(phone)) {
        throw ValidationException("Phone number must contain 10-15 digits.");
    }
    m_phone = phone;
}

void Person::setGender(const std::string& gender) {
    if (gender.empty()) {
        throw ValidationException("Gender cannot be empty.");
    }
    m_gender = gender;
}

// Static validation helpers
bool Person::isValidEmail(const std::string& email) {
    const std::regex emailPattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return std::regex_match(email, emailPattern);
}

bool Person::isValidPhone(const std::string& phone) {
    // Allows optional +, digits, spaces, and hyphens (min 7, max 15 digits)
    const std::regex phonePattern(R"(^\+?[0-9\-\s]{7,15}$)");
    return std::regex_match(phone, phonePattern);
}
