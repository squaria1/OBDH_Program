/**
 * \file controlMode.c
 * \brief control mode functions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * control mode functions
 *
 */
#include "init.h"

#ifdef _WIN32
#else
/**
 * \brief function to send telemetry to the TT&C subsystem
 *
 * \return statusErrDef that values:
 * - errWriteCANTelem when the CAN frame can't be written,
 * - noError when the function exits successfully.
 */
statusErrDef sendTelemToTTC(const statusErrDef statusErr) {
	statusErrDef ret = noError;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;

	printf("sendTelemToTTC test \r\n");

	frame.can_id = 0x555;
	frame.can_dlc = 5;
	sprintf(frame.data, "0x%04X", statusErr);

	if (write(socket_fd, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
		perror("errWriteCANTelem");
		return errWriteCANTelem;
	}

	return ret;
}

/**
 * \brief function to recieve telecommands from the TT&C subsystem
 *
 * \return statusErrDef that values:
 * - errReadCANTC when CAN frame can't be read,
 * - noError when the function exits successfully.
 */
statusErrDef recieveTCFromTTC() {
	statusErrDef ret = noError;
	int s, i;
	int nbytes;
	struct can_frame frame;

	printf("recieveTCFromTTC test \r\n");

	nbytes = read(s, &frame, sizeof(struct can_frame));

	if (nbytes < 0) {
		perror("errReadCANTC");
		return errReadCANTC;
	}

	printf("0x%03X [%d] ", frame.can_id, frame.can_dlc);

	for (i = 0; i < frame.can_dlc; i++)
		printf("%02X ", frame.data[i]);

	printf("\r\n");

	return ret;
}
#endif

statusErrDef checkSensors() {
	statusErrDef ret = noError;
	return ret;
}

statusErrDef checkTC() {
	statusErrDef ret = noError;
	return ret;
}
