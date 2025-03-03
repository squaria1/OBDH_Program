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
#include <cstring>
#include <cstdlib>
#include <arpa/inet.h>
#include <fcntl.h>

#include <linux/can.h>
#include <linux/can/raw.h>

//------------------------------------------------------------------------------
// Global function definitions
//------------------------------------------------------------------------------
statusErrDef initOBDH();
statusErrDef initAOCS();
statusErrDef initTTC();
statusErrDef initPayload();
statusErrDef initIntersat();
statusErrDef initEPS();
statusErrDef initPPU();

//------------------------------------------------------------------------------
// Global structure definitions
//------------------------------------------------------------------------------
/**
 * \struct paramSensorsStruct
 * \brief struct containing values taken from "paramSensors.csv"
 *
 */
struct paramSensorsStruct {
    uint16_t id[MAX_SENSORS];                   /**< In the form like 0x1902 Where 1 is the payload subsystem and 2 is the third sensor of the subsystem */
    int32_t minCriticalValue[MAX_SENSORS];      /**< Minimum critical value of the sensor */
    int32_t minWarnValue[MAX_SENSORS];          /**< Minimum warning value of the sensor */
    int32_t currentValue[MAX_SENSORS];          /**< Current value of the sensor */
    int32_t maxWarnValue[MAX_SENSORS];          /**< Maximum warning value of the sensor */
    int32_t maxCriticalValue[MAX_SENSORS];      /**< Maximum critical value of the sensor */
};


//------------------------------------------------------------------------------
// global vars
//------------------------------------------------------------------------------
extern int lineCountSensorParamCSV;
extern int socket_can;
extern int socket_udp;
extern struct paramSensorsStruct* paramSensors;

#endif
