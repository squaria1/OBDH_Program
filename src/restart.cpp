/**
 * \file restart.cpp
 * \brief Subsystem initialisation functions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Subsystem initialisation functions
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
	free(paramSensors);
	return ret;
}

/**
 * \brief function to free variables of the AOCS subsystem
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef freeAOCS() {
	statusErrDef ret = noError;
	return ret;
}

/**
 * \brief function to free variables of the TT&C subsystem
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef freeTTC() {
	statusErrDef ret = noError;
	ret = closeUDPSocket();
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
	return ret;
}

/**
 * \brief function to free variables of the EPS subsystem
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef freeEPS() {
	statusErrDef ret = noError;
	return ret;
}

/**
 * \brief function to free variables of the PPU subsystem
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef freePPU() {
	statusErrDef ret = noError;
	return ret;
}
