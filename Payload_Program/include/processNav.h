/**
 * \file processNav.h
 * \brief Subsystem process navigation requests function definitions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Contains all of the process navigation requests function definitions
 */

#ifndef PROCESSNAV_H
#define PROCESSNAV_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "configDefine.h"
#include "statesDefine.h"

statusErrDef calcNavFromDopplerShift();
statusErrDef sendNavToUser();

#endif


