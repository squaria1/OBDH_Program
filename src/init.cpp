/**
 * \file init.c
 * \brief Subsystem initialisation functions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Subsystem initialisation functions
 *
 */
#include "init.h"

int socket_fd = 0;
int socket_udp = 0;

statusErrDef initCANSocket() {
	statusErrDef ret = noError;
	struct sockaddr_can addr;
	struct ifreq ifr;

	printf("CAN Sockets init\r\n");

	if ((socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("errCreateCANSocket");
		return errCreateCANSocket;
	}

	int buf_size = CAN_SOCKET_BUFFER_SIZE;
	if(setsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size)) == -1){
		perror("errSetCANSocketBufSize");
		return errSetCANSocketBufSize;
	}

	int flags = fcntl(socket_fd, F_GETFL, 0);
	if(flags == -1) {
		perror("errGetCANSocketFlags");
		return errGetCANSocketFlags;
	}
    if(fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("errSetCANSocketNonBlocking");
		return errSetCANSocketNonBlocking;
	}


	strcpy(ifr.ifr_name, CAN_INTERFACE);
	ioctl(socket_fd, SIOCGIFINDEX, &ifr);

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("errBindCANAddr");
		return errBindCANAddr;
	}
	return ret;
}

statusErrDef initUDPSocket() {
	statusErrDef ret = noError;
    struct sockaddr_in serverAddr;

    // Create UDP socket
    socket_udp = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_udp < 0) {
        perror("Socket creation failed");
        return errCreateUDPSocket;
    }

	int buf_size = UDP_SOCKET_BUFFER_SIZE;
	if(setsockopt(socket_udp, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size)) == -1){
		perror("errSetUDPSocketBufSize");
		return errSetUDPSocketBufSize;
	}

	int flags = fcntl(socket_udp, F_GETFL, 0);
	if(flags == -1) {
		perror("errGetUDPSocketFlags");
		return errGetUDPSocketFlags;
	}
    if(fcntl(socket_udp, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("errSetUDPSocketNonBlocking");
		return errSetUDPSocketNonBlocking;
	}

    // Bind to UDP port
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(UDP_TELECOMMAND_PORT);

    if (bind(socket_udp, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        return errBindUDPAddr;
    }


    printf("Listening for cFS packets on UDP port %d ...\n", UDP_TELECOMMAND_PORT);
	return ret;
}

/**
 * \brief function to initialize the OBDH subsystem
 *
 * \return statusErrDef that values:
 * - errCreateCANSocket when CAN socket can't be created,
 * - errBindCANAddr when CAN address can't be bind,
 * - noError when the function exits successfully.
 */
statusErrDef initOBDH() {
	statusErrDef ret = noError;
	ret = initCANSocket();
	return ret;
}

/**
 * \brief function to initialize the AOCS subsystem
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef initAOCS() {
	statusErrDef ret = noError;
	return ret;
}

/**
 * \brief function to initialize the TT&C subsystem
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef initTTC() {
	statusErrDef ret = noError;
	ret = initUDPSocket();
	return ret;
}

/**
 * \brief function to initialize the Payload subsystem
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef initPayload() {
	statusErrDef ret = noError;
	return ret;
}

/**
 * \brief function to initialize the Intersat laser subsystem
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef initIntersat() {
	statusErrDef ret = noError;
	return ret;
}

/**
 * \brief function to initialize the EPS subsystem
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef initEPS() {
	statusErrDef ret = noError;
	return ret;
}

/**
 * \brief function to initialize the PPU subsystem
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef initPPU() {
	statusErrDef ret = noError;
	return ret;
}
