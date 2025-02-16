/**
 * \file controlMode.h
 * \brief controlMode function definitions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Contains all of the controlMode function definitions
 */

#ifndef CONTROLMODE_H
#define CONTROLMODE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "configDefine.h"
#include "statesDefine.h"

#ifdef _WIN32
#else
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>
#endif

#ifdef _WIN32
#else
statusErrDef sendTelemToTTC(const statusErrDef statusErr);
statusErrDef recieveTCFromTTC();
#endif

statusErrDef checkSensors();
statusErrDef checkTC();

/**
 * \struct TCBacklog
 * \brief struct containing the history of TCs
 *
 */
struct TCBacklog {
    int receivedDate;                              /**< the set value of a valve */
};

#endif