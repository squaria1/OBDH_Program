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
            if (ret == noError) {
                    printf("No 5G packet received\n");
            }
            else if (ret == info5GPacketReceived) {
                state = processMsg;
            }
            else if (ret == infoRecieve5GPacketsTimeout) {
                state = idleMode;
            }
            else {
                printf("Error recieving 5G packets! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }

            ret = recieveNavReq();
            if (ret == noError) {
                printf("No nav request received\n");
            }
            else if (ret == infoNavReqReceived) {
                state = processNav;
            }
            else {
                printf("Error recieving navigation request! 0x%04X \n", ret);
                sendTelemToOBDH(ret);
            }

            sleep(MAIN_LOOP_TIME);
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
