#include "StudentManager.h"
#include "Exceptions.h"

#include <iostream>
#include <exception>

int main() {
    try {
        // Instantiate StudentManager (Demonstrates Object Creation & Composition)
        StudentManager manager;
        
        // Run main menu loop
        manager.run();

        return 0;
    } catch (const StudentManagementException& e) {
        std::cerr << "\n[CRITICAL ERROR] Student Management Exception: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "\n[CRITICAL ERROR] Standard Exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "\n[CRITICAL ERROR] An unknown fatal error occurred.\n";
        return 1;
    }
}
