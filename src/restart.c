/**
 * \file init.c
 * \brief Subsystem initialisation functions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Subsystem initialisation functions
 *
 */
#include "init.h"

statusErrDef closeCANSocket() {
	statusErrDef ret = noError;
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
