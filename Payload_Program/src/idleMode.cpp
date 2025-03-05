/**
 * \file idleMode.cpp
 * \brief Subsystem idle mode functions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Subsystem idle mode functions
 *
 */

#include "idleMode.h"

//------------------------------------------------------------------------------
// Local vars definition
//------------------------------------------------------------------------------
/**
 * \brief Determine if low power is activated or not.
 */
static bool isLowPower = false;

//------------------------------------------------------------------------------
// Local function definitions
//------------------------------------------------------------------------------
/**
 * \brief function to check if a 5G packet has
 * been recieved and put the payload Subsystem
 * in low power mode.
 *
 * \return statusErrDef that values:
 * - info5GPacketReceived when a 5G packet has been recieved
 * - noError when the function exits successfully.
 */
statusErrDef recieve5GPacketsIdle() {
	statusErrDef ret = noError;
	ssize_t sizeReceived = 0;
	if(sizeReceived > 0) {
        isLowPower = false;
		return info5GPacketReceived;
	}
	else {
        isLowPower = true;
	}
	return ret;
}


