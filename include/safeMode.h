/**
 * \file safeMode.h
 * \brief safeMode function definitions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Contains all of the safeMode function definitions
 */

#ifndef SAFEMODE_H
#define SAFEMODE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "configDefine.h"
#include "statesDefine.h"

statusErrDef stopPayload();
statusErrDef broadcastSafeMode();

#endif
