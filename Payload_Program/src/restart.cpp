/**
 * \file restart.cpp
 * \brief Subsystem restart functions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Subsystem restart functions
 *
 */
#include "init.h"
#include "restart.h"

//------------------------------------------------------------------------------
// Local function definitions
//------------------------------------------------------------------------------
statusErrDef closeCANSocket();
statusErrDef closeUDPSocket();

//------------------------------------------------------------------------------
// Local function definitions
//------------------------------------------------------------------------------
/**
 * \brief function to close the CAN socket
 *
 * \return statusErrDef that values:
 * - errCloseCANSocket when the CAN socket can't be closed
 * - noError when the function exits successfully.
 */
statusErrDef closeCANSocket() {
	statusErrDef ret = noError;
	if (close(socket_can) < 0) {
		perror("errCloseCANSocket");
		return errCloseCANSocket;
	}
	return ret;
}

/**
 * \brief function to close the UDP socket
 *
 * \return statusErrDef that values:
 * - errCloseUDPSocket when the UDP socket can't be closed
 * - noError when the function exits successfully.
 */
statusErrDef closeUDPSocket() {
	statusErrDef ret = noError;
	if (close(socket_udp) < 0) {
		perror("errCloseUDPSocket");
		return errCloseUDPSocket;
	}
	return ret;
}

/**
 * \brief function to free variables of the OBDH subsystem
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef freeOBDH() {
	statusErrDef ret = noError;
	ret = closeCANSocket();
	return ret;
}

/**
 * \brief function to free variables of the Payload subsystem
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef freePayload() {
	statusErrDef ret = noError;
	return ret;
}

/**
 * \brief function to free variables of the Intersat laser subsystem
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef freeIntersat() {
	statusErrDef ret = noError;
	ret = closeUDPSocket();
	return ret;
}
