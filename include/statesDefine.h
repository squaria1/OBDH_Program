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

	// Safe mode (from 0x0020 to 0x003F)

	// Control mode (from 0x0040 to 0x005F)

	// Restart (from 0x00E0 to 0x00FF)

	// Main state (from 0x0700 to 0x07FF)
	infoStateToInit = 0x0700,				/**< The main state has been changed to initialisation. */
	infoStateToSafeMode = 0x0701,			/**< The main state has been changed to acquisition and control. */
	infoStateToControlMode = 0x0702,		/**< The main state has been changed to control mode. */
	infoStateToRegulate = 0x0703,			/**< The main state has been changed to regulate state. */
	infoStateToRestart = 0x07FF,			/**< The main state has been changed to restart state. */

	/*=============ERROR===============*/
	// Init (from 0x0E00 to 0x0E1F)
	errCreateCANSocket = 0x0E01,			/**<  */
	errSetCANSocketBufSize = 0x0E02,		/**<  */
	errGetCANSocketFlags = 0x0E03,			/**<  */
	errSetCANSocketNonBlocking = 0x0E04,	/**<  */
	errBindCANAddr = 0x0E05,				/**<  */
	errCreateUDPSocket = 0x0E06,			/**<  */
	errSetUDPSocketBufSize = 0x0E07,		/**<  */
	errGetUDPSocketFlags = 0x0E08,			/**<  */
	errSetUDPSocketNonBlocking = 0x0E09,	/**<  */
	errBindUDPAddr = 0x0E0A,				/**<  */

	// Safe mode (from 0x0E20 to 0x0E3F)

	// Control mode (from 0x0E40 to 0x0E5F)
	errWriteCANTelem = 0x0E20,				/**<  */
	errReadCANPayload = 0x0E21,				/**<  */
	errWriteCANPayload = 0x0E22,			/**<  */
	errSensorOutOfBounds = 0x0E23,			/**<  */
	errCCSDSPacketTooLarge = 0x0E24,		/**<  */
	errWriteUDPTelem = 0x0E25,				/**<  */

	// Restart (from 0x0EE0 to 0x0EFF)
	errCloseCANSocket = 0x0EF0,				/**<  */
	errCloseUDPSocket = 0x0EF1,				/**<  */

} statusErrDef;

/**
 * \enum stateDef
 * \brief list of states for the main state machine
 */
typedef enum
{
	init = 0x0700, 									/**< Initialize all modules with status and error telemetry. */
	safeMode = 0x0701, 								/**< Activate valves and read sensors values. */
	controlMode = 0x0702,
	regulate = 0x0703,
	restart = 0x07FF, 								/**< Shutdown all modules with status and error telemetry. */
	ending = 0x0FFF, 								/**< Stop the program. */
} stateDef;

/**
 * \enum busTypeDef
 * \brief list of connection type
 */
typedef enum
{
	CANType, 									/**< */
	UDPType, 									/**< */
} busTypeDef;

/**
 * \enum sensorDef
 * \brief list of connection type
 */
typedef enum
{
	sensor1 = 0x0900,				/**<  */
	sensor2 = 0x0901,				/**<  */
	sensor3 = 0x0902,				/**<  */
} sensorDef;

#endif
