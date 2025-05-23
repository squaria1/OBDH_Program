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

#if USE_VCAN
#else
	char sys_cmd_can[CAN_CMD_LENGHT];
	sprintf(sys_cmd_can, "sudo ip link set %s down", CAN_INTERFACE);
    system(sys_cmd_can);
#endif

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
	free(paramSensors->id);
	free(paramSensors->minCriticalValue);
	free(paramSensors->minWarnValue);
	free(paramSensors->currentValue);
	free(paramSensors->maxWarnValue);
	free(paramSensors->maxCriticalValue);
	free(paramSensors);
	paramSensors = NULL;
	for(int i = 0; i < lineCountSensorParamCSV; i++) {
		fclose(sensorsVal[i].sensorFile);
		sensorsVal[i].sensorFile = NULL; // Prevent dangling pointer
	}
	if (sensorsVal != NULL) {
        free(sensorsVal);
        sensorsVal = NULL;
    }
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
