/**
 * \file statesDefine.h
 * \brief header file containing all of the program enumerations
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Contains all of the program enumerations namely, the program
 * status and error codes, the program modes and the state machine states.
 */

#ifndef STATESDEFINE_H
#define STATESDEFINE_H


#include <unordered_set>
#include <stdint.h>

 /**
  * \enum statusErrDef
  * \brief the program status and error codes
  *
  */
typedef enum
{
	noError = 0x0000,						/**< No error has been detected, the function is working. */

	/*=====INFORMATION AND SUCCESS=====*/
	// Init (from 0x0000 to 0x001F)
	infoInitOBDHSuccess = 0x1000,			/**< The main state machine OBDH subsystem initialisation has succeeded. */
	infoInitPayloadSuccess = 0x1001,		/**< The main state machine Payload subsystem initialisation has succeeded. */

	// Safe mode (from 0x0020 to 0x003F)
	infoSendStopPayloadSuccess = 0x1020,	/**< The payload stop order has been successfully sent. */
	infoBroadcastSafeModeSuccess = 0x1021,	/**< The safe mode has been sent to all subsystems. */

	// Payload mode (from 0x0040 to 0x005F)
	info5GPacketReceived = 0x1040,			/**<  */
	infoRecieve5GPacketsTimeout = 0x1041,	/**<  */
	infoNavReqReceived = 0x1042,			/**<  */

	// Restart (from 0x00E0 to 0x00FF)
	infoFreePayloadSuccess = 0x10E0,		/**< Payload subsystem memory freeing has succeeded.  */
	infoFreeOBDHSuccess = 0x10E1,			/**< OBDH subsystem memory freeing has succeeded. */

	// Main state (from 0x0700 to 0x07FF)
	infoStateToInit = 0x1700,				/**< The main state has been changed to initialisation. */
	infoStateToPayloadMode = 0x1701,		/**< The main state has been changed to payload mode. */
	infoStateToRestart = 0x17FF,			/**< The main state has been changed to restart state. */

	/*=============ERROR===============*/
	// Init (from 0x0E00 to 0x0E1F)
	errCreateCANSocket = 0x1E00,			/**< CAN socket creation failed. */
	errSetCANSocketBufSize = 0x1E01,		/**< Set CAN socket buffer size failed. */
	errGetCANSocketFlags = 0x1E02,			/**< Get CAN socket flags failed. */
	errSetCANSocketNonBlocking = 0x1E03,	/**< Set CAN socket to non-blocking (doesn't wait for an incoming frame to continue). */
	errBindCANAddr = 0x1E04,				/**< Bind CAN address to the CAN socket failed. */
	errAllocParamSensorStruct = 0x1E05,		/**< paramSensors structure memory allocation failed. */
	errOpenParamSensorsFile = 0x1E06,		/**< paramSensors.csv file not found or unable to read. */

	// Safe mode (from 0x0E20 to 0x0E3F)

	// Control mode (from 0x0E40 to 0x0E5F)
	errReadCANPayload = 0x1E20,				/**< Read CAN bus payload subsystem telemetry failed. */
	errWriteCANPayload = 0x1E21,			/**< Write payload subsystem TCs to the CAN bus failed. */
	errReadCANEPS = 0x1E22,					/**< Read CAN bus EPS subsystem telemetry failed. */
	errCCSDSPacketTooLarge = 0x1E23,		/**< Error CCSDS packet is too large for the CAN FD frame (64 Bytes). */
	errSensorWarningValue = 0x1E24,			/**< A sensor has reached a minimum or maximum warning value from the paramSensors.csv file. */
	errSensorCriticalValue = 0x1E25,		/**< A sensor has reached a minimum or maximum critical value from the paramSensors.csv file. */

	// Restart (from 0x0EE0 to 0x0EFF)
	errCloseCANSocket = 0x1EF0,				/**< close CAN socket failed. */

} statusErrDef;

/**
 * \enum stateDef
 * \brief list of states for the main state machine
 */
typedef enum
{
	init = 0x1700, 									/**< Initialize all subsystems with status and error telemetry. */
	payloadMode = 0x1702,							/**< Spacecraft normal operation (check TCs, send telemetry, compare sensor values). */
	processMsg = 0x1703,							/**< Process the 5G packet(s) recieved (redirect to Intersat subsystem or to the ground station). */
	idleMode = 0x1704,								/**< Activate the low power mode when no 5G packets have been recieved after a certain time. */
	processNav = 0x1705,							/**< Process the navigation request to find the user location from the doppler shift algorithm */
	restart = 0x17FF, 								/**< Free all subsystems variables, systemd script will restart the program when ending. */
	ending = 0x1FFF, 								/**< Stop the program. */
} stateDef;

const std::unordered_set<uint16_t> validStates = {
    init, payloadMode, processMsg, idleMode, processNav, restart, ending
};

#endif
