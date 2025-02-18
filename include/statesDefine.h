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
	// Init (from 0x0000 to 0x00FF)

	// Safe mode (from 0x0100 to 0x01FF)

	// Control mode (from 0x0200 to 0xE2FF)

	// Restart (from 0x0F00 to 0x0FFF)

	// Main state (from 0x7000 to 0x7FFF)
	infoStateToInit = 0x7001,				/**< The main state has been changed to initialisation. */
	infoStateToSafeMode = 0x7002,			/**< The main state has been changed to acquisition and control. */
	infoStateToControlMode = 0x7003,		/**< The main state has been changed to control mode. */
	infoStateToRegulate = 0x7004,			/**< The main state has been changed to regulate state. */
	infoStateToRestart = 0x7FFF,			/**< The main state has been changed to restart state. */

	/*=============ERROR===============*/
	// Init (from 0xE000 to 0xE0FF)
	errCreateCANSocket = 0xE001,			/**<  */
	errBindCANAddr = 0xE002,				/**<  */

	// Safe mode (from 0xE100 to 0xE1FF)

	// Control mode (from 0xE200 to 0xE2FF)
	errWriteCANTelem = 0xE201,				/**<  */
	errReadCANTC = 0xE202,					/**<  */
	errSensorOutOfBounds = 0xE203,			/**<  */

	// Restart (from 0xEF00 to 0xEFFF)
	errCloseCANSocket = 0xEF01				/**<  */

} statusErrDef;

/**
 * \enum stateDef
 * \brief list of states for the main state machine
 */
typedef enum
{
	init, 								/**< Initialize all modules with status and error telemetry. */
	safeMode, 							/**< Activate valves and read sensors values. */
	controlMode,
	regulate,
	restart, 							/**< Shutdown all modules with status and error telemetry. */
	ending, 							/**< Stop the program. */
} stateDef;



#endif
