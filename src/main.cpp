/**
 * \file main.cpp
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
    uint16_t        mainStateTCTemp = 0xFFFF;

    while (state != ending) {
        switch (state) {
        case init: // OBDH and subsystem connection initialisation
            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initTTC();
                if (ret == noError) {
                    printf("init TT&C OK\n");
                    sendTelemToTTC(infoInitTTCSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init TT&C! 0x%04X \n", ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initOBDH();
                if (ret == noError) {
                    printf("init OBDH OK\n");
                    sendTelemToTTC(infoInitOBDHSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init OBDH! 0x%04X \n", ret);
                    sendTelemToTTC(ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initAOCS();
                if (ret == noError) {
                    printf("init AOCS OK\n");
                    sendTelemToTTC(infoInitAOCSSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init AOCS! 0x%04X \n", ret);
                    sendTelemToTTC(ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initEPS();
                if (ret == noError) {
                    printf("init EPS OK\n");
                    sendTelemToTTC(infoInitEPSSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init EPS! 0x%04X \n", ret);
                    sendTelemToTTC(ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initPayload();
                if (ret == noError) {
                    printf("init Payload OK\n");
                    sendTelemToTTC(infoInitPayloadSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init Payload! 0x%04X \n", ret);
                    sendTelemToTTC(ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initIntersat();
                if (ret == noError) {
                    printf("init Intersat OK\n");
                    sendTelemToTTC(infoInitIntersatSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init Intersat! 0x%04X \n", ret);
                    sendTelemToTTC(ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initPPU();
                if (ret == noError) {
                    printf("init PPU OK\n");
                    sendTelemToTTC(infoInitPPUSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init PPU! 0x%04X \n", ret);
                    sendTelemToTTC(ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            printf("State has been changed to control mode\n");
            sendTelemToTTC(infoStateToControlMode);
            state = controlMode;
            break;
        case safeMode: // Enter safe mode procedure with telecommand or unable to regulate
            ret = stopPayload();
            if (ret == noError)
                printf("Send stop to payload OK\n");
            else
                printf("Error send stop to payload! Ox%04X \n", ret);
            break;
            ret = broadcastSafeMode();
            if (ret == noError)
                printf("Send safe mode to all subsystems OK\n");
            else
                printf("Error send safe mode to all subsystems! 0x%04X \n", ret);
            break;
        case controlMode: // Control subsystems (sensor acquisition, telemetry to/TC from TT&C)
            /*
            ret = checkSensors();
            if (ret == noError)
                printf("Sensor check OK\n");
            else if (ret == errSensorOutOfBounds) {
                ret = sendTelemToTTC(infoStateToRegulate);
                if (ret == noError)
                    printf("sendTelemToTTC infoStateToRegulate OK\n");
                else
                    printf("Error sendTelemToTTC infoStateToRegulate! 0x%04X \n", ret);
                state = regulate;
            }
            else
                printf("Error sensor check!\n");
            */
            ret = checkTC();
            if (ret == noError)
                printf("check TC backlog OK\n");
            else
                printf("Error check TC backlog! 0x%04X \n", ret);
            sleep(MAIN_LOOP_TIME);
            break;
        case regulate: // Regulate subsystems when sensor out of bounds
            sendTelemToTTC(infoStateToRegulate);
            printf("\n\n\nREGULATE STATE\n\n\n");
            state = controlMode;
            break;
        case restart: // Restart program with systemd
            sendTelemToTTC(infoStateToRestart);
            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freePPU();
                if (ret == noError) {
                    printf("free PPU OK\n");
                    sendTelemToTTC(infoFreePPUSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free PPU! 0x%04X \n", ret);
                    sendTelemToTTC(ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freeIntersat();
                if (ret == noError) {
                    printf("free Intersat OK\n");
                    sendTelemToTTC(infoFreeIntersatSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free Intersat! 0x%04X \n", ret);
                    sendTelemToTTC(ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freePayload();
                if (ret == noError) {
                    printf("free Payload OK\n");
                    sendTelemToTTC(infoFreePayloadSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free Payload! 0x%04X \n", ret);
                    sendTelemToTTC(ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freeAOCS();
                if (ret == noError) {
                    printf("free AOCS OK\n");
                    sendTelemToTTC(infoFreeAOCSSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free AOCS! 0x%04X \n", ret);
                    sendTelemToTTC(ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freeEPS();
                if (ret == noError) {
                    printf("free EPS OK\n");
                    sendTelemToTTC(infoFreeEPSSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free EPS! 0x%04X \n", ret);
                    sendTelemToTTC(ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freeOBDH();
                if (ret == noError) {
                    printf("free OBDH OK\n");
                    sendTelemToTTC(infoFreeOBDHSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free OBDH! 0x%04X \n", ret);
                    sendTelemToTTC(ret);
                    sleep(ERROR_RETRY_TIME);
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
                    printf("Error free TT&C! 0x%04X \n", ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }
            state = ending;
            break;
        default:
            break;
        }
        if(mainStateTC != 0xFFFF && mainStateTC != mainStateTCTemp &&
            (mainStateTC & 0xF000) == 0x0000 && validStates.count(mainStateTC)) {
            state = static_cast<stateDef>(mainStateTC);
            mainStateTCTemp = mainStateTC;
        }
    }

    return 0;
}
