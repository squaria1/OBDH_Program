/**
 * \file main.cpp
 * \brief Main program with state machine
 * \author Mael Parot
 * \version 1.0
 * \date 16/05/2025
 *
 * Main program of the Payload subsytem
 *
 */

#include "configDefine.h"
#include "statesDefine.h"
#include "init.h"
#include "payloadMode.h"
#include "processMsg.h"
#include "idleMode.h"
#include "processNav.h"
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
        case init: // Payload and subsystem connection initialisation
            // Retry to initialize NB_RETRIES times with ERROR_RETRY_TIME delay between each retries
            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initOBDH();
                if (ret == noError) {
                    printf("init OBDH OK\n");
                    sendTelemToOBDH(infoInitOBDHSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init OBDH! 0x%04X \n", ret);
                    sendTelemToOBDH(ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initPayload();
                if (ret == noError) {
                    printf("init Payload OK\n");
                    sendTelemToOBDH(infoInitPayloadSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init Payload! 0x%04X \n", ret);
                    sendTelemToOBDH(ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = initIntersat();
                if (ret == noError) {
                    printf("init Intersat OK\n");
                    sendTelemToOBDH(infoInitIntersatSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error init Intersat! 0x%04X \n", ret);
                    sendTelemToOBDH(ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            printf("State has been changed to control mode\n");
            sendTelemToOBDH(infoStateToPayloadMode);
            state = payloadMode;
            break;
        case payloadMode:
            ret = checkTC();
            if (ret == noError) {
                    printf("Check TC OK\n");
            }
            else {
                printf("Error check TC! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }

            ret = recieve5GPackets();
            if (ret == info5GPacketReceived) {
                state = processMsg;
            }
            else if (ret == infoRecieve5GPacketsTimeout) {
                sendTelemToOBDH(infoStateToIdleMode);
                state = idleMode;
            }
            else {
                printf("Error recieving 5G packets! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }

            ret = recieveNavReq();
            if (ret == infoNavReqReceived) {
                state = processNav;
            }
            else {
                printf("Error recieving navigation request! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }

            sleep(MAIN_LOOP_TIME);
            break;
        case processMsg:
            ret = aStarPathAlgorithm();
            if (ret == infoDirectPathToGS) {
                ret = transmitToGS();
                if (ret != noError) {
                    printf("Error transmitting 5G packet to the ground station! 0x%04X \n", ret);
                    sendTelemToOBDH(ret);
                }
            }
            else if (ret == infoPathToNextNode) {
                ret = transmitToIntersat({0x11, 0x22});
                if (ret != noError) {
                    printf("Error transmitting 5G packet to the intersatellite subsystem! 0x%04X \n", ret);
                    sendTelemToOBDH(ret);
                }
            }
            else {
                printf("Error in A star path algorithm! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }

            state = payloadMode;
            break;
        case idleMode:
            ret = recieve5GPacketsIdle();
            if (ret == info5GPacketReceived) {
                state = processMsg;
            }
            else {
                printf("Error in Idle loop! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }
            break;
        case processNav:
            state = payloadMode;
            break;
        case restart: // Restart program with systemd
            sendTelemToOBDH(infoStateToRestart);

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freeOBDH();
                if (ret == noError) {
                    printf("free OBDH OK\n");
                    sendTelemToOBDH(infoFreeOBDHSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free OBDH! 0x%04X \n", ret);
                    sendTelemToOBDH(ret);
                    sleep(ERROR_RETRY_TIME);
                    retryCounter++;
                }
            }

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freePayload();
                if (ret == noError) {
                    printf("free Payload OK\n");
                    sendTelemToOBDH(infoFreePayloadSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free Payload! 0x%04X \n", ret);
                    sendTelemToOBDH(ret);
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
            (mainStateTC & 0xF000) == 0x1000 && validStates.count(mainStateTC)) {
            state = static_cast<stateDef>(mainStateTC);
            mainStateTCTemp = mainStateTC;
        }
    }

    return 0;
}
