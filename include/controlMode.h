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
statusErrDef sendTCToPayload(std::vector<uint8_t> TCOut);
statusErrDef sendTelemToTTC(const statusErrDef statusErr);
statusErrDef sendSensorDataToTTC(const sensorDef sensor, std::vector<uint8_t> sensorValue);
statusErrDef recieveTCFromTTC();
statusErrDef checkSensors();
statusErrDef checkTC();


//------------------------------------------------------------------------------
// global vars
//------------------------------------------------------------------------------
extern uint16_t mainStateTC;

#endif
