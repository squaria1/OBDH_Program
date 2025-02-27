/**
 * \file payloadMode.h
 * \brief payload mode function definitions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Contains all of the payload mode function definitions
 */

#ifndef PAYLOADMODE_H
#define PAYLOADMODE_H

#include "CCSDSLibrary/CCSDS.hh"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstring>
#include "configDefine.h"
#include "statesDefine.h"

#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

std::vector<uint8_t> generateCCSDSPacket(std::vector<uint8_t> dataOut);
statusErrDef sendTelemToOBDH(const statusErrDef statusErr);
statusErrDef recieveTCFromOBDH();
statusErrDef checkTC();
statusErrDef recieve5GPackets();
statusErrDef recieveNavReq();
void resetMsgTimer();


//------------------------------------------------------------------------------
// global vars
//------------------------------------------------------------------------------
extern uint16_t mainStateTC;

#endif
