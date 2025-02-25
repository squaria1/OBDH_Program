/**
 * \file init.cpp
 * \brief Subsystem initialisation functions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Subsystem initialisation functions
 *
 */
#include "init.h"

/**
 * \brief the CAN socket global variable.
 */
int socket_can = 0;

/**
 * \brief function to initialize the CAN socket
 *
 * \return statusErrDef that values:
 * - errCreateCANSocket when the CAN socket creation fails
 * - errSetCANSocketBufSize when the CAN socket buffer size cannot be applied
 * - errGetCANSocketFlags CAN socket flags cannot be read
 * - errSetCANSocketNonBlocking when the CAN socket non blocking flag cannot be set
 * - errBindCANAddr when the CAN address cannot be bound to the CAN socket
 * - noError when the function exits successfully.
 */
statusErrDef initCANSocket() {
	statusErrDef ret = noError;
	struct sockaddr_can addr;
	struct ifreq ifr;

	printf("CAN Sockets init\r\n");

	if ((socket_can = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("errCreateCANSocket");
		return errCreateCANSocket;
	}

	int buf_size = CAN_SOCKET_BUFFER_SIZE;
	if(setsockopt(socket_can, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size)) == -1){
		perror("errSetCANSocketBufSize");
		return errSetCANSocketBufSize;
	}

	int flags = fcntl(socket_can, F_GETFL, 0);
	if(flags == -1) {
		perror("errGetCANSocketFlags");
		return errGetCANSocketFlags;
	}
    if(fcntl(socket_can, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("errSetCANSocketNonBlocking");
		return errSetCANSocketNonBlocking;
	}


	strcpy(ifr.ifr_name, CAN_INTERFACE);
	ioctl(socket_can, SIOCGIFINDEX, &ifr);

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(socket_can, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("errBindCANAddr");
		return errBindCANAddr;
	}
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
 * \brief function to initialize the Payload subsystem
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef initPayload() {
	statusErrDef ret = noError;
	return ret;
}
