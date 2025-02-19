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

#define CAN_INTERFACE "vcan0"
#define CAN_SOCKET_BUFFER_SIZE 100000 //in bytes
#define CAN_ID_OBDH 0x100
#define UDP_SOCKET_BUFFER_SIZE 100000
#define DATA_OUT_CAN_MAX_LENGTH 48
#define NB_RETRIES 3
#define UDP_TELEMETRY_PORT 5000
#define UDP_TELECOMMAND_PORT 5001
#define UDP_MAX_BUFFER_SIZE 1024
#define TTC_IP_ADDRESS "127.0.0.1"


#endif
