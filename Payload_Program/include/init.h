/**
 * \file init.h
 * \brief Subsystem initialisation function definitions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Contains all of the subsystem initialisation function definitions
 */

#ifndef INIT_H
#define INIT_H

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
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <time.h>

#include <linux/can.h>
#include <linux/can/raw.h>

statusErrDef initCANSocket();
statusErrDef initUDPSocket();
statusErrDef initOBDH();
statusErrDef initPayload();
statusErrDef initIntersat();


//------------------------------------------------------------------------------
// global vars
//------------------------------------------------------------------------------
extern int socket_can;
extern int socket_udp;
extern struct timespec beginSensorSamplingTimer;
extern struct timespec endSensorSamplingTimer;

#endif
