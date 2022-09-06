cmake_minimum_required(VERSION 3.5)
project(WindowsSignalHandlerBugCandidate)

if(WIN32)
    set(CMAKE_SUPPRESS_REGENERATION ON)
    add_executable(WindowsWriteCrossPage src/WindowsWriteCrossPage.cpp)
else()
    add_executable(UnixWriteCrossPage src/UnixWriteCrossPage.cpp)
endif()
