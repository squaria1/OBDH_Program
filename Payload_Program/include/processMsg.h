/**
 * \file processMsg.h
 * \brief Subsystem process 5G packets function definitions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Contains all of the process 5G packets function definitions
 */

#ifndef PROCESSMSG_H
#define PROCESSMSG_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <cstdint>
#include "configDefine.h"
#include "statesDefine.h"

#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>

statusErrDef aStarPathAlgorithm();
statusErrDef transmitToGS();
statusErrDef transmitToIntersat(std::vector<uint8_t> packet);

#endif

