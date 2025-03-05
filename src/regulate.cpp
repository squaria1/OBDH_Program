/**
 * \file regulate.cpp
 * \brief regulate sensor values functions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Regulate sensor values functions
 *
 */
#include "regulate.h"
#include "controlMode.h"
#include "init.h"

//------------------------------------------------------------------------------
// Local function definitions
//------------------------------------------------------------------------------
/**
 * \brief function to trigger regulation procedures
 * in the spacecraft sensor(s) location(s).
 *
 * \return statusErrDef that values:
 * - errSensorCriticalValue when a sensor has reached a minimum or maximum warning value from the paramSensors.csv file.
 * - noError when the function exits successfully.
 */
statusErrDef regulateSubsystems() {
    statusErrDef ret = noError;
    for(int i = 1; i < lineCountSensorParamCSV; i++) {
		// Check if the sensor current value is out of its warning bounds
		if(paramSensors->currentValue[i] <= paramSensors->minWarnValue[i] ||
			paramSensors->currentValue[i] >= paramSensors->maxWarnValue[i]) {
				// Check if the sensor current value is out of its critical bounds
				if(paramSensors->currentValue[i] <= paramSensors->minCriticalValue[i] ||
				paramSensors->currentValue[i] >= paramSensors->maxCriticalValue[i]) {
					sendTelemToTTC(errSensorCriticalValue);
					return errSensorCriticalValue;
				}
				else {
                    //TODO
				}
			}
	}

	return ret;
}
