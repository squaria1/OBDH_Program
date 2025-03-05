/**
 * \file restart.h
 * \brief restart subsystem function definitions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Contains all of the restart subsystem function definitions
 */

#ifndef RESTART_H
#define RESTART_H

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

statusErrDef freeOBDH();
statusErrDef freePayload();
statusErrDef freeIntersat();

#endif
