cmake_minimum_required(VERSION 3.10)

project(OBDH_PROGRAM CXX)

MESSAGE(STATUS "Configuring OBDH_PROGRAM")

set(CMAKE_CXX_STANDARD 11)      # Use C++11
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Enforce the standard
set(CMAKE_CXX_EXTENSIONS OFF)   # Use standard C++ (disable compiler-specific extensions)

SET(OBDH_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
SET(OBDH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
SET(OBDH_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

SET(OBDH_SOURCES
    ${OBDH_SOURCE_DIR}/main.cpp
    ${OBDH_SOURCE_DIR}/init.cpp
    ${OBDH_SOURCE_DIR}/controlMode.cpp
    ${OBDH_SOURCE_DIR}/regulate.cpp
    ${OBDH_SOURCE_DIR}/restart.cpp
    ${OBDH_SOURCE_DIR}/safeMode.cpp
    )

INCLUDE_DIRECTORIES(
	${OBDH_INCLUDE_DIR}
	${OBDH_INCLUDE_DIR}/CCSDSLibrary
    )


add_executable(OBDH_Program ${OBDH_SOURCES})
