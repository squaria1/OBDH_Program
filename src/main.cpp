/**
 * \file main.c
 * \brief Main program with state machine
 * \author Mael Parot
 * \version 1.0
 * \date 16/05/2025
 *
 * Main program of the OBDH subsytem
 *
 */

#include "configDefine.h"
#include "statesDefine.h"
#include "init.h"
#include "controlMode.h"
#include "safeMode.h"
#include "regulate.h"
#include "restart.h"

 /**
  * \brief main function of the program
  *
  * \return 0 if the program exits properly
  */
int main() {
    stateDef        state = init;
    statusErrDef    ret = noError;
    int             retryCounter = 0;

    while (state != ending) {
        switch (state) {
        case init: // OBDH and subsystems initialisation
            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initOBDH();
                if (ret == noError) {
                    printf("init OBDH OK\n");
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init OBDH!\n");
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initAOCS();
                if (ret == noError) {
                    printf("init AOCS OK\n");
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init AOCS!\n");
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initTTC();
                if (ret == noError) {
                    printf("init TT&C OK\n");
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init TT&C!\n");
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initPayload();
                if (ret == noError) {
                    printf("init Payload OK\n");
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init Payload!\n");
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initIntersat();
                if (ret == noError) {
                    printf("init Intersat OK\n");
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init Intersat!\n");
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initEPS();
                if (ret == noError) {
                    printf("init EPS OK\n");
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init EPS!\n");
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initPPU();
                if (ret == noError) {
                    printf("init PPU OK\n");
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init PPU!\n");
                    retryCounter++;
                }
            }

            printf("State has been changed to control mode\n");
            ret = sendTelemToTTC(infoStateToControlMode);
            if (ret == noError)
                printf("sendTelemToTTC infoStateToControlMode OK\n");
            else
                printf("Error sendTelemToTTC infoStateToControlMode!\n");
            state = controlMode;
            break;
        case safeMode: // Enter safe mode procedure with telecommand or unable to regulate
            break;
        case controlMode: // Control subsystems (sensor acquisition, telemetry to/TC from TT&C)
            ret = checkSensors();
            if (ret == noError)
                printf("Sensor check OK\n");
            else if (ret == errSensorOutOfBounds) {
                ret = sendTelemToTTC(infoStateToRegulate);
                if (ret == noError)
                    printf("sendTelemToTTC infoStateToRegulate OK\n");
                else
                    printf("Error sendTelemToTTC infoStateToRegulate!\n");
                state = regulate;
            }
            else
                printf("Error sensor check!\n");
            ret = checkTC();
            if (ret == noError)
                printf("check TC backlog OK\n");
            else
                printf("Error check TC backlog!\n");
            state = restart;
            break;
        case regulate: // Regulate subsystems when sensor out of bounds
            break;
        case restart: // Restart program with systemd
            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freePPU();
                if (ret == noError) {
                    printf("free PPU OK\n");
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free PPU!\n");
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freeEPS();
                if (ret == noError) {
                    printf("free EPS OK\n");
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free EPS!\n");
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freeIntersat();
                if (ret == noError) {
                    printf("free Intersat OK\n");
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free Intersat!\n");
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freePayload();
                if (ret == noError) {
                    printf("free Payload OK\n");
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free Payload!\n");
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freeTTC();
                if (ret == noError) {
                    printf("free TT&C OK\n");
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free TT&C!\n");
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freeAOCS();
                if (ret == noError) {
                    printf("free AOCS OK\n");
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free AOCS!\n");
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freeOBDH();
                if (ret == noError) {
                    printf("free OBDH OK\n");
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free OBDH!\n");
                    retryCounter++;
                }
            }
            state = ending;
            break;
        default:
            break;
        }
    }

    return 0;
}
