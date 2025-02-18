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

#include "CCSDSLibrary/CCSDS.hh"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "configDefine.h"
#include "statesDefine.h"

#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

statusErrDef processCCSDSPacket();
statusErrDef sendTelemToTTC(const statusErrDef statusErr);
statusErrDef recieveTCFromTTC();

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
