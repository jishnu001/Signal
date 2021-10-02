cmake_minimum_required(VERSION 3.0)
project(Test)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Locate the necessary files
set(SOURCES main.cc)
set(HEADERS SomeClass.h)

# Add the executable target
add_executable(test ${SOURCES} ${HEADERS})
