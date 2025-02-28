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
 * \brief State machine main loop delay in nanoseconds.
 */
#define MAIN_LOOP_TIME 20000000L

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
 * \brief Broadcast CAN ID in 12 bits.
 */
#define CAN_ID_BROADCAST 0xFFF

/**
 * \brief UDP socket buffer size in bytes
 * (fills up when frame are received but not read).
 */
#define UDP_SOCKET_BUFFER_SIZE 100000

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
 * \brief NASA cFS program TT&C subsystem telemetry UDP port.
 */
#define UDP_TELEMETRY_PORT 5000

/**
 * \brief NASA cFS program TT&C subsystem telecommands UDP port.
 */
#define UDP_TELECOMMAND_PORT 5001

/**
 * \brief Maximum amount of data per UDP frame in bytes.
 */
#define UDP_MAX_BUFFER_SIZE 1024

/**
 * \brief TT&C subsytem IP address for telemetry and
 * telecommands with the OBDH subsystem.
 */
#define TTC_IP_ADDRESS "127.0.0.1"

/**
 * \brief Maximum number of sensors in the spacecraft
 * (limited by sensor ID encoding: 0x0900 to 0xE9FF).
 */
#define MAX_SENSORS 3840

/**
 * \brief Maximum number of characters per column in
 * the paramSensors.csv file.
 */
#define MAX_CSV_LINE_SIZE 256

/**
 * \brief paramSensors.csv file path
 * (CSV file containing every spacecraft sensors parameters).
 */
#define PARAM_SENSORS_CSV_FILEPATH "../paramFiles/paramSensors.csv"


#endif
