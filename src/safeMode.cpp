/**
 * \file safeMode.cpp
 * \brief Safe mode functions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Safe mode functions
 *
 */
#include "safeMode.h"
#include "controlMode.h"
#include "init.h"

//------------------------------------------------------------------------------
// Local function definitions
//------------------------------------------------------------------------------
/**
 * \brief function to send the safe mode state
 * to every subsystems.
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef broadcastSafeMode() {
    statusErrDef ret = noError;
    ret = sendTCToSubsystem({0xF7,0x01}, everySubsystems);
    return ret;
}
