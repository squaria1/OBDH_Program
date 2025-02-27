cmake_minimum_required(VERSION 3.10)

project(PAYLOAD_PROGRAM CXX)

MESSAGE(STATUS "Configuring PAYLOAD_PROGRAM")

set(CMAKE_CXX_STANDARD 11)      # Use C++11
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Enforce the standard
set(CMAKE_CXX_EXTENSIONS OFF)   # Use standard C++ (disable compiler-specific extensions)

SET(PAYLOAD_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
SET(PAYLOAD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
SET(PAYLOAD_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

SET(PAYLOAD_SOURCES
    ${PAYLOAD_SOURCE_DIR}/main.cpp
    ${PAYLOAD_SOURCE_DIR}/init.cpp
    ${PAYLOAD_SOURCE_DIR}/payloadMode.cpp
    ${PAYLOAD_SOURCE_DIR}/processMsg.cpp
    ${PAYLOAD_SOURCE_DIR}/idleMode.cpp
    ${PAYLOAD_SOURCE_DIR}/processNav.cpp
    ${PAYLOAD_SOURCE_DIR}/restart.cpp
    )

INCLUDE_DIRECTORIES(
	${PAYLOAD_INCLUDE_DIR}
	${PAYLOAD_INCLUDE_DIR}/CCSDSLibrary
    )


add_executable(Payload_Program ${PAYLOAD_SOURCES})
