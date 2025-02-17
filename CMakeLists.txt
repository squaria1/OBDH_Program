cmake_minimum_required(VERSION 3.10)

project(OBDH_PROGRAM)

MESSAGE(STATUS "Configuring OBDH_PROGRAM")

SET(OBDH_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
SET(OBDH_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
SET(OBDH_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

SET(OBDH_SOURCES
    ${OBDH_SOURCE_DIR}/main.c
    ${OBDH_SOURCE_DIR}/init.c
    ${OBDH_SOURCE_DIR}/controlMode.c
    ${OBDH_SOURCE_DIR}/regulate.c
    ${OBDH_SOURCE_DIR}/restart.c
    ${OBDH_SOURCE_DIR}/safeMode.c
    )

INCLUDE_DIRECTORIES(
	${OBDH_INCLUDE_DIR}
    )


add_executable(OBDH_Program ${OBDH_SOURCES})
