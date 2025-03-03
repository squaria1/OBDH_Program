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

//------------------------------------------------------------------------------
// Global enumeration definitions
//------------------------------------------------------------------------------
 /**
  * \enum statusErrDef
  * \brief the program status and error codes
  *
  */
typedef enum
{
	noError = 0x0000,						/**< No error has been detected, the function is working. */

	/*=====INFORMATION AND SUCCESS=====*/
	// Init (from 0x1000 to 0x101F)
	infoInitOBDHSuccess = 0x1000,			/**< The main state machine OBDH subsystem initialisation has succeeded. */
	infoInitPayloadSuccess = 0x1001,		/**< The main state machine Payload subsystem initialisation has succeeded. */
	infoInitIntersatSuccess = 0x1002,		/**< The main state machine Intersatellite comms subsystem initialisation has succeeded. */

	// Safe mode (from 0x1020 to 0x103F)
	infoSendStopPayloadSuccess = 0x1020,	/**< The payload stop order has been successfully sent. */
	infoBroadcastSafeModeSuccess = 0x1021,	/**< The safe mode has been sent to all subsystems. */

	// Payload mode (from 0x1040 to 0x105F)
	info5GPacketReceived = 0x1040,			/**< A 5G packet has been recieved. */
	infoRecieve5GPacketsTimeout = 0x1041,	/**< No 5G packet has been recieved after a certain time. */
	infoNavReqReceived = 0x1042,			/**< A navigation request has been recieved. */

	// Process msg (from 0x1060 to 0x107F)
	infoDirectPathToGS = 0x1060,			/**< The 5G packet recieved can be transmitted directly to the ground station. */
	infoPathToNextNode = 0x1061,			/**< The 5G packet recieved has to be transfered to another constellation satellite (node). */

	// Idle mode (from 0x1080 to 0x109F)

	// Process nav (from 0x10A0 to 0x10BF)

	// Restart (from 0x10E0 to 0x10FF)
	infoFreeIntersatSuccess = 0x00E0,		/**< Intersatellite comms subsystem memory freeing has succeeded. */
	infoFreeOBDHSuccess = 0x10E1,			/**< OBDH subsystem memory freeing has succeeded. */
	infoFreePayloadSuccess = 0x10E2,		/**< Payload subsystem memory freeing has succeeded.  */

	// Main state (from 0x1700 to 0x17FF)
	infoStateToInit = 0x1700,				/**< The main state has been changed to initialisation. */
	infoStateToPayloadMode = 0x1701,		/**< The main state has been changed to payload mode. */
	infoStateToProcessMsg = 0x1702,			/**< The main state has been changed to process 5G packets. */
	infoStateToIdleMode = 0x1703,			/**< The main state has been changed to idle mode. */
	infoStateToProcessNav = 0x1704,			/**< The main state has been changed to process navigation request. */
	infoStateToRestart = 0x17FF,			/**< The main state has been changed to restart state. */

	/*=============ERROR===============*/
	// Init (from 0x1E00 to 0x1E1F)
	errCreateCANSocket = 0x1E00,			/**< CAN socket creation failed. */
	errSetCANSocketBufSize = 0x1E01,		/**< Set CAN socket buffer size failed. */
	errGetCANSocketFlags = 0x1E02,			/**< Get CAN socket flags failed. */
	errSetCANSocketNonBlocking = 0x1E03,	/**< Set CAN socket to non-blocking (doesn't wait for an incoming frame to continue). */
	errBindCANAddr = 0x1E04,				/**< Bind CAN address to the CAN socket failed. */
	errCreateUDPSocket = 0x1E05,			/**< UDP socket creation failed. */
	errSetUDPSocketBufSize = 0x1E06,		/**< Set UDP socket buffer size failed. */
	errGetUDPSocketFlags = 0x1E07,			/**< Get UDP socket flags failed. */
	errSetUDPSocketNonBlocking = 0x1E08,	/**< Set UDP socket to non-blocking (doesn't wait for an incoming frame to continue). */
	errBindUDPAddr = 0x1E09,				/**< Bind UDP address to the UDP socket failed. */
	errAllocParamSensorStruct = 0x1E0A,		/**< paramSensors structure memory allocation failed. */
	errOpenParamSensorsFile = 0x1E0B,		/**< paramSensors.csv file not found or unable to read. */

	// Safe mode (from 0x1E20 to 0x1E3F)

	// Control mode (from 0x1E40 to 0x1E5F)
	errReadCANPayload = 0x1E20,				/**< Read CAN bus payload subsystem telemetry failed. */
	errWriteCANPayload = 0x1E21,			/**< Write payload subsystem TCs to the CAN bus failed. */
	errReadCANEPS = 0x1E22,					/**< Read CAN bus EPS subsystem telemetry failed. */
	errCCSDSPacketTooLarge = 0x1E23,		/**< Error CCSDS packet is too large for the CAN FD frame (64 Bytes). */
	errSensorWarningValue = 0x1E24,			/**< A sensor has reached a minimum or maximum warning value from the paramSensors.csv file. */
	errSensorCriticalValue = 0x1E25,		/**< A sensor has reached a minimum or maximum critical value from the paramSensors.csv file. */

	// Process msg (from 0x1E60 to 0x1E7F)
	errWriteUDPIntersat = 0x1E60,			/**< Send 5G packet to Intersatellite subsystem through UDP failed. */

	// Idle mode (from 0x1E80 to 0x1E9F)

	// Process nav (from 0x1EA0 to 0x1EBF)

	// Restart (from 0x1EE0 to 0x1EFF)
	errCloseCANSocket = 0x1EF0,				/**< close CAN socket failed. */
	errCloseUDPSocket = 0x1EF1,				/**< close UDP socket failed. */

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
