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

#include <signal.h>
#include "configDefine.h"
#include "statesDefine.h"
#include "init.h"
#include "payloadMode.h"
#include "processMsg.h"
#include "idleMode.h"
#include "processNav.h"
#include "restart.h"

stateDef                state = init;

 /**
  * \brief Exit the program gracefully (freeing all
  * allocations) when killing or terminating or
  * CTRL+C the process.
  */
void handle_signal(int sig) {
    if (sig == SIGINT)
        printf("Caught SIGINT\n");
    else if (sig == SIGTERM)
        printf("Caught SIGTERM\n");
    else if (sig == SIGKILL)
        printf("Caught SIGKILL\n");
    state = restart;
}

 /**
  * \brief main function of the program
  *
  * \return 0 if the program exits properly
  */
int main() {
    statusErrDef    ret = noError;
    int             retryCounter = 0;
    uint16_t        mainStateTCTemp = 0xFFFF;
    struct timespec mainSleep = {0, MAIN_LOOP_TIME};

    // Register the signal handlers
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
    signal(SIGKILL, handle_signal);

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

            printf("State has been changed to payload mode\n");
            sendTelemToOBDH(infoStateToPayloadMode);
            resetMsgTimer();
            state = payloadMode;
            break;
        case payloadMode:
            ret = checkTC();
            if (ret == noError || ret == infoNoDataInCANBuffer) {
                //printf("Check TC OK\n");
            }
            else {
                printf("Error check TC! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }

            ret = checkSensors();
            if (ret == noError) {
                //printf("Check sensors OK\n");
            }
            else {
                printf("Error check sensors! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }

            ret = recieve5GPackets();
            if(ret == noError) {
                //printf("No 5G packet recieved.\n");
            }
            else if (ret == info5GPacketReceived) {
                sendTelemToOBDH(infoStateToProcessMsg);
                state = processMsg;
            }
            else if (ret == infoRecieve5GPacketsTimeout) {
                printf("State has been changed to idle mode\n");
                sendTelemToOBDH(infoStateToIdleMode);
                state = idleMode;
            }
            else {
                printf("Error recieving 5G packets! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }

            ret = recieveNavReq();
            if(ret == noError) {
                //printf("No nav request recieved.\n");
            }
            else if (ret == infoNavReqReceived) {
                sendTelemToOBDH(infoStateToProcessNav);
                state = processNav;
            }
            else {
                printf("Error recieving navigation request! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }

            nanosleep(&mainSleep, NULL);
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

            resetMsgTimer();
            sendTelemToOBDH(infoStateToPayloadMode);
            state = payloadMode;
            break;
        case idleMode:
            ret = checkTC();
            if (ret == noError || ret == infoNoDataInCANBuffer) {
                //printf("Check TC OK\n");
            }
            else {
                printf("Error check TC! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }

            ret = checkSensors();
            if (ret == noError) {
                //printf("Check sensors OK\n");
            }
            else {
                printf("Error check sensors! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }

            ret = recieve5GPacketsIdle();
            if(ret == noError) {
                //printf("No 5G packet recieved in Idle mode.\n");
            }
            else if (ret == info5GPacketReceived) {
                sendTelemToOBDH(infoStateToProcessMsg);
                state = processMsg;
            }
            else {
                printf("Error in Idle loop! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }

            nanosleep(&mainSleep, NULL);
            break;
        case processNav:
            ret = calcNavFromDopplerShift();
            if (ret != noError) {
                printf("Error calculating user position from doppler shift! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }

            ret = sendNavToUser();
            if (ret != noError) {
                printf("Error sending navigation data to the user! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }

            resetMsgTimer();
            sendTelemToOBDH(infoStateToPayloadMode);
            state = payloadMode;
            break;
        case restart: // Restart program with systemd
            sendTelemToOBDH(infoStateToRestart);

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freeIntersat();
                if (ret == noError) {
                    printf("free Intersat OK\n");
                    sendTelemToOBDH(infoFreeIntersatSuccess);
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free Intersat! 0x%04X \n", ret);
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

            retryCounter = 0;
            while (retryCounter < NB_RETRIES) {
                ret = freeOBDH();
                if (ret == noError) {
                    printf("free OBDH OK\n");
                    retryCounter = NB_RETRIES;
                }
                else {
                    printf("Error free OBDH! 0x%04X \n", ret);
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
