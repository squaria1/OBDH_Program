/**
 * \file processMsg.cpp
 * \brief Subsystem process 5G packets functions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Subsystem process 5G packets functions
 *
 */

#include "processMsg.h"
#include "init.h"

//------------------------------------------------------------------------------
// Local function definitions
//------------------------------------------------------------------------------
/**
 * \brief function to check if a ground station
 * is currently connected to the spacecraft.
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef checkGSConnected() {
	statusErrDef ret = noError;

    //TODO

    return ret;
}

/**
 * \brief function to calculate the path of least
 * distance from the user to the closest ground station
 * and vis versa.
 *
 * \return statusErrDef that values:
 * - infoDirectPathToGS when the 5G packet recieved can be transmitted directly to the ground station.
 * - infoPathToNextNode when the 5G packet recieved has to be transfered to another constellation satellite (node).
 * - noError when the function exits successfully.
 */
statusErrDef aStarPathAlgorithm() {
	statusErrDef ret = noError;
    int nbNodes = 0;

    //TODO

    if(nbNodes == 1) {
        return infoDirectPathToGS;
    }
    else if(nbNodes >= 1) {
        return infoPathToNextNode;
    }

	return ret;
}

/**
 * \brief function to transmit the 5G packet to the
 * ground station through the payload antenna.
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef transmitToGS() {
    statusErrDef ret = noError;

    //TODO

	return ret;
}

/**
 * \brief function to transmit the 5G packet to the
 * intersatellite subsystem.
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef transmitToIntersat(std::vector<uint8_t> packet) {
    statusErrDef ret = noError;

	// The destination address (this is where the packet will be sent)
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(UDP_PAYLOAD_TO_INTERSAT_PORT);  // Destination port
    clientAddr.sin_addr.s_addr = inet_addr(INTERSAT_IP_ADDRESS);

    // Send the packet over UDP
    ssize_t bytes_sent = sendto(socket_udp, packet.data(), packet.size(),
                            0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
    if (bytes_sent < 0) {
        perror("errWriteUDPIntersat");
        ret = errWriteUDPIntersat;  // Error in sending packet
    }

	return ret;
}
