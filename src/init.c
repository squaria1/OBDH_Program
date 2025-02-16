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

#ifdef _WIN32
#else
statusErrDef initCANSocket() {
	statusErrDef ret = noError;
	int s, i;
	int nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;

	printf("CAN Sockets init\r\n");

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("errCreateCANSocket");
		return errCreateCANSocket;
	}

	strcpy(ifr.ifr_name, "vcan0");
	ioctl(s, SIOCGIFINDEX, &ifr);

	memset(&addr, 0, sizeof(addr));
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("errBindCANAddr");
		return errBindCANAddr;
	}
	return ret;
}
#endif

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
#ifdef _WIN32
#else
	ret = initCANSocket();
#endif
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