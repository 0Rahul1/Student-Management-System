cmake_minimum_required(VERSION 3.15)
project(StudentManagementSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Source and Include setup
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
    src/Person.cpp
    src/Student.cpp
    src/Course.cpp
    src/Attendance.cpp
    src/Result.cpp
    src/Database.cpp
    src/StudentManager.cpp
    src/main.cpp
)

add_executable(StudentManagementSystem ${SOURCES})

# Attempt to locate MongoDB C++ Driver
find_package(mongocxx CONFIG QUIET)
find_package(bsoncxx CONFIG QUIET)

if(TARGET mongo::mongocxx_shared AND TARGET mongo::bsoncxx_shared)
    message(STATUS "[MongoDB] Found mongo::mongocxx_shared and mongo::bsoncxx_shared targets.")
    target_link_libraries(StudentManagementSystem PRIVATE mongo::mongocxx_shared mongo::bsoncxx_shared)
elseif(TARGET mongo::mongocxx_static AND TARGET mongo::bsoncxx_static)
    message(STATUS "[MongoDB] Found mongo::mongocxx_static and mongo::bsoncxx_static targets.")
    target_link_libraries(StudentManagementSystem PRIVATE mongo::mongocxx_static mongo::bsoncxx_static)
else()
    # Fallback to module / manual search
    find_package(mongocxx QUIET)
    find_package(bsoncxx QUIET)

    if(mongocxx_FOUND AND bsoncxx_FOUND)
        message(STATUS "[MongoDB] Found mongocxx and bsoncxx via module mode.")
        target_include_directories(StudentManagementSystem PRIVATE ${LIBMONGOCXX_INCLUDE_DIRS} ${LIBBSONCXX_INCLUDE_DIRS})
        target_link_libraries(StudentManagementSystem PRIVATE ${LIBMONGOCXX_LIBRARIES} ${LIBBSONCXX_LIBRARIES})
    else()
        message(WARNING "[MongoDB] mongocxx driver targets not found automatically.")
        message(STATUS "To build with MongoDB C++ driver via vcpkg on Windows, use:")
        message(STATUS "cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-mingw-dynamic")
    endif()
endif()

# On Windows MinGW, link winsock and crypto
if(WIN32)
    target_link_libraries(StudentManagementSystem PRIVATE ws2_32 crypt32 secur32)
endif()
