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
	// Init (from 0x0000 to 0x001F)
	infoInitTTCSuccess = 0x0000,			/**< The main state machine TT&C subsystem initialisation has succeeded. */
	infoInitOBDHSuccess = 0x0001,			/**< The main state machine OBDH subsystem initialisation has succeeded. */
	infoInitEPSSuccess = 0x0002,			/**< The main state machine EPS subsystem initialisation has succeeded. */
	infoInitAOCSSuccess = 0x0003,			/**< The main state machine AOCS subsystem initialisation has succeeded. */
	infoInitPayloadSuccess = 0x0004,		/**< The main state machine Payload subsystem initialisation has succeeded. */
	infoInitIntersatSuccess = 0x0005,		/**< The main state machine Intersatellite comms subsystem initialisation has succeeded. */
	infoInitPPUSuccess = 0x0006,			/**< The PPU (propulsion system Power Processing Unit) subsystem initialisation has succeeded. */

	// Safe mode (from 0x0020 to 0x003F)
	infoSendStopPayloadSuccess = 0x0020,	/**< The payload stop order has been successfully sent. */
	infoBroadcastSafeModeSuccess = 0x0021,	/**< The safe mode has been sent to all subsystems. */

	// Control mode (from 0x0040 to 0x005F)
	infoNoDataInCANBuffer = 0x0040,			/**< No data has been recieved through the CAN bus from the subsystems. */

	// Restart (from 0x00E0 to 0x00FF)
	infoFreePPUSuccess = 0x00E0,			/**< PPU (propulsion system Power Processing Unit) subsystem memory freeing has succeeded. */
	infoFreeIntersatSuccess = 0x00E1,		/**< Intersatellite comms subsystem memory freeing has succeeded. */
	infoFreePayloadSuccess = 0x00E2,		/**< Payload subsystem memory freeing has succeeded.  */
	infoFreeAOCSSuccess = 0x00E3,			/**< AOCS subsystem memory freeing has succeeded. */
	infoFreeEPSSuccess = 0x00E4,			/**< EPS subsystem memory freeing has succeeded. */
	infoFreeOBDHSuccess = 0x00E5,			/**< OBDH subsystem memory freeing has succeeded. */
	infoFreeTTCSuccess = 0x00E6,			/**< TT&C subsystem memory freeing has succeeded. */

	// Main state (from 0x0700 to 0x07FF)
	infoStateToInit = 0x0700,				/**< The main state has been changed to initialisation. */
	infoStateToSafeMode = 0x0701,			/**< The main state has been changed to acquisition and control. */
	infoStateToControlMode = 0x0702,		/**< The main state has been changed to control mode. */
	infoStateToRegulate = 0x0703,			/**< The main state has been changed to regulate state. */
	infoStateToRestart = 0x07FF,			/**< The main state has been changed to restart state. */

	/*=============ERROR===============*/
	// Init (from 0x0E00 to 0x0E1F)
	errCreateCANSocket = 0x0E00,			/**< CAN socket creation failed. */
	errEnableCANFD = 0x0E01,				/**< enable CAN FD mode failed. */
	errSetCANSocketBufSize = 0x0E02,		/**< Set CAN socket buffer size failed. */
	errGetCANSocketFlags = 0x0E03,			/**< Get CAN socket flags failed. */
	errSetCANSocketNonBlocking = 0x0E04,	/**< Set CAN socket to non-blocking (doesn't wait for an incoming frame to continue). */
	errBindCANAddr = 0x0E05,				/**< Bind CAN address to the CAN socket failed. */
	errCreateUDPSocket = 0x0E06,			/**< UDP socket creation failed. */
	errSetUDPSocketBufSize = 0x0E07,		/**< Set UDP socket buffer size failed. */
	errGetUDPSocketFlags = 0x0E08,			/**< Get UDP socket flags failed. */
	errSetUDPSocketNonBlocking = 0x0E09,	/**< Set UDP socket to non-blocking (doesn't wait for an incoming frame to continue). */
	errBindUDPAddr = 0x0E0A,				/**< Bind UDP address to the UDP socket failed. */
	errAllocParamSensorStruct = 0x0E0B,		/**< paramSensors structure memory allocation failed. */
	errOpenParamSensorsFile = 0x0E0C,		/**< paramSensors.csv file not found or unable to read. */

	// Safe mode (from 0x0E20 to 0x0E3F)

	// Control mode (from 0x0E40 to 0x0E5F)
	errReadCANTelem = 0x0E20,				/**< Write payload subsystem TCs to the CAN bus failed. */
	errCCSDSPacketTooLarge = 0x0E21,		/**< Error CCSDS packet is too large for the CAN FD frame (64 Bytes). */
	errWriteUDPTelem = 0x0E22,				/**< Send OBDH telemetry CCSDS packet through UDP failed. */
	errWriteCANTC = 0x0E23,					/**< Write CCSDS TC packet to the CAN bus failed. */
	errSensorWarningValue = 0x0E24,			/**< A sensor has reached a minimum or maximum warning value from the paramSensors.csv file. */
	errSensorCriticalValue = 0x0E25,		/**< A sensor has reached a minimum or maximum critical value from the paramSensors.csv file. */
	errTCToWrongSubsystem = 0x0E26,			/**< Trying to send a telecommand to a subsystem that should not recieve any. */
	errCCSDSPacketUninterpretable = 0x0E27,	/**< The CCSDS packet recieved cannot be interpreted (wrong sequence or corrupted data). */

	// Restart (from 0x0EE0 to 0x0EFF)
	errCloseCANSocket = 0x0EF0,				/**< close CAN socket failed. */
	errCloseUDPSocket = 0x0EF1,				/**< close UDP socket failed. */

} statusErrDef;

/**
 * \enum stateDef
 * \brief list of states for the main state machine
 */
typedef enum
{
	init = 0x0700, 							/**< Initialize all subsystems with status and error telemetry. */
	safeMode = 0x0701, 						/**< Spacecraft to safe mode (payload shutdown and all subsystems to safe mode). */
	controlMode = 0x0702,					/**< Spacecraft normal operation (check TCs, send telemetry, compare sensor values). */
	regulate = 0x0703,						/**< Regulation procedures when one or more sensors reach warning values. */
	restart = 0x07FF, 						/**< Free all subsystems variables, systemd script will restart the program when ending. */
	ending = 0x0FFF, 						/**< Stop the program. */
} stateDef;

const std::unordered_set<uint16_t> validStates = {
    init, safeMode, controlMode, regulate, restart, ending
};

/**
 * \enum sensorDef
 * \brief list of the spacecraft sensors
 */
typedef enum
{
	sensor1 = 0x0900,						/**<  */
	sensor2 = 0x0901,						/**<  */
	sensor3 = 0x0902,						/**<  */
} sensorDef;

/**
 * \enum subsystemDef
 * \brief list of the spacecraft subsystems
 */
typedef enum
{
	OBDHSubsystem = 0x0000,					/**< On Board Data Handling subsystem */
	payloadSubsystem = 0x1000,				/**< Payload subsystem (communication service) */
	EPSSubsystem = 0x2000,					/**< Electrical Power System subsystem */
	everySubsystems = 0xF000,				/**< To broadcast to all subsystems */
} subsystemDef;

#endif
