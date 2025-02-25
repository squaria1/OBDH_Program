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

#ifndef CONFIGDEFINE_H
#define CONFIGDEFINE_H

/**
 * \brief State machine main loop delay in seconds.
 */
#define MAIN_LOOP_TIME 1

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
constexpr double MSG_TIMEOUT = 120.5;



#endif
