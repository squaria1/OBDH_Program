/**
 * \file configDefine.h
 * \brief header file that contains all the program defines
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Contains all of the program defines from each module.
 * The defines are set at compile time so they can't be changed
 * at runtime.
 */

/** @mainpage Payload Program documentation
 * @section intro Introduction
 * <a href="https://github.com/squaria1/OBDH_Program">
 * the OBDH Program Github repository</a>,
 *
 * @section install Installation and Build
 * @subsection requirements program requirements
 *
 * - cmake (>v3.10)
 * - g++
 * - git
 * - linux environnement (or VM, WSL)
 *
 *
 * @subsection Optional Optional
 * - Doxygen (for documentation generation)
 *
 * @subsection Packages Packages installation
 * Install required packages,
 * @code
 * sudo apt install cmake build-essential doxygen git
 * @endcode
 *
 * Clone from the github repository,
 * @code
 * cd ~
 * git clone https://github.com/squaria1/OBDH_Program.git
 * @endcode
 *
 * @subsection Build Build
 * @code
 * cd ~/OBDH_Program/Payload_Program
 * mkdir build
 * cd build
 * cmake -S ../ -B .
 * make
 * @endcode
 *
 * Run the Payload program,
 * @code
 * sudo ./OBDH_Program
 * @endcode
 *
 * (Optional) Generate Payload program documentation with Doxygen,
 * @code
 * cd ../doc
 * doxygen Doxyfile
 * @endcode
 *
 * Then open html/index.html with a web browser.
 *
 */

#ifndef CONFIGDEFINE_H
#define CONFIGDEFINE_H

//------------------------------------------------------------------------------
// Global define program parameters (change as you wish)
//------------------------------------------------------------------------------
/**
 * \brief State machine main loop delay in nanoseconds.
 */
#define MAIN_LOOP_TIME 10000000L

/**
 * \brief delay between each initialisation or freeing
 * error retries in seconds.
 */
#define ERROR_RETRY_TIME 1

/**
 * \brief CAN device name in the Linux device management system.
 */
#define CAN_INTERFACE "vcan0"

/**
 * \brief CAN socket buffer size in bytes
 * (fills up when frame are received but not read).
 */
#define CAN_SOCKET_BUFFER_SIZE 100000

/**
 * \brief UDP socket buffer size in bytes
 * (fills up when frame are received but not read).
 */
#define UDP_SOCKET_BUFFER_SIZE 100000

/**
 * \brief OBDH CAN ID in 12 bits.
 */
#define CAN_ID_OBDH 0x100

/**
 * \brief Payload CAN ID in 12 bits.
 */
#define CAN_ID_PAYLOAD 0x200

/**
 * \brief Maximum CCSDS user data accounting for the
 * CCSDS primary and secondary headers
 * (64 Bytes maximum for a CAN FD frame).
 */
#define DATA_OUT_CAN_MAX_LENGTH 48

/**
 * \brief Number of initialisation or freeing error retries.
 */
#define NB_RETRIES 3

/**
 * \brief When a 5G packet has not been recieved after
 * MSG_TIMEOUT seconds.
 */
constexpr double MSG_TIMEOUT = 20.5;

#define INTERSAT_IP_ADDRESS "192.168.1.3"

#define UDP_PAYLOAD_TO_INTERSAT_PORT 4010

#define UDP_INTERSAT_TO_PAYLOAD_PORT 4020



#endif
