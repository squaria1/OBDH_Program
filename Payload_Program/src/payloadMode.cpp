/**
 * \file payloadMode.cpp
 * \brief payload mode functions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * payload mode functions
 *
 */
#include "payloadMode.h"
#include "init.h"

uint8_t counter = 0;
uint16_t mainStateTC = 0xFFFF;

/**
 * \brief beginning references of the dependance timers.
 */
struct timespec beginMsgTimer;
/**
 * \brief ending references of the dependance timers.
 */
struct timespec endMsgTimer;

/**
 * \brief function to send telemetry to the TT&C subsystem
 *
 * \param dataOut the data to transmit or recieve from a CAN frame,
 * cut in a vector of bytes
 *
 * \return a CCSDS packet to be sent
 */
std::vector<uint8_t> generateCCSDSPacket(std::vector<uint8_t> dataOut) {
	uint16_t apid = 0x1AB;
	//uint8_t category = 0;
	//uint8_t aduCount = 0;
	size_t sequenceCount = 1;

	//constructs an empty instance
	CCSDSSpacePacket ccsdsPacketIN;
	//set APID
	ccsdsPacketIN.getPrimaryHeader()->setAPID(apid);
	//set Packet Type (Telemetry or Command)
	ccsdsPacketIN.getPrimaryHeader()->setPacketType(CCSDSSpacePacketPacketType::TelemetryPacket);
	//set Secondary Header Flag (whether this packet has the Secondary Header part)
	ccsdsPacketIN.getPrimaryHeader()->setSecondaryHeaderFlag(CCSDSSpacePacketSecondaryHeaderFlag::NotPresent);
	//set segmentation information
	ccsdsPacketIN.getPrimaryHeader()->setSequenceFlag(CCSDSSpacePacketSequenceFlag::UnsegmentedUserData);
	//set Category
	//ccsdsPacketIN->getSecondaryHeader()->setCategory(category);
	//set secondary header type (whether ADU Channel presence)
	//ccsdsPacketIN->getSecondaryHeader()->
	//setSecondaryHeaderType(CCSDSSpacePacketSecondaryHeaderType::ADUChannelIsUsed);
	//set ADU Channel ID
	//ccsdsPacketIN->getSecondaryHeader()->setADUChannelID(0x00);
	//set ADU Segmentation Flag (whether ADU is segmented)
	//ccsdsPacketIN->getSecondaryHeader()->setADUSegmentFlag(CCSDSSpacePacketADUSegmentFlag::UnsegmentedADU);
	//set counters
	ccsdsPacketIN.getPrimaryHeader()->setSequenceCount(sequenceCount);
	//ccsdsPacketIN->getSecondaryHeader()->setADUCount(aduCount);
	//set absolute time
	//uint8_t time[4];
	//ccsdsPacketIN->getSecondaryHeader()->setTime(time);
	//set data
	ccsdsPacketIN.setUserDataField(dataOut);
	ccsdsPacketIN.setPacketDataLength();
	//get packet as byte array
	std::vector<uint8_t> packet = ccsdsPacketIN.getAsByteVector();

	/*
	printf("\n\n\n==================================================\n\n\n");

	//constructs an empty instance
	CCSDSSpacePacket ccsdsPacket;

	//interpret an input data as a CCSDS SpacePacket
	ccsdsPacket.interpret(packet.data(),packet.size());
	//check if the packet has Secondary Header
	if(ccsdsPacket.isSecondaryHeaderPresent()){
		printf("HAS A SECONDARY HEADER\n");
	}
	//get APID
	std::cout << ccsdsPacket.getPrimaryHeader()->getAPIDAsInteger() << std::endl;
	//dump packet content
	std::cout << ccsdsPacket.toString() << std::endl;
	*/

	return packet;
}


/**
 * \brief function to recieve telecommands from the TT&C subsystem
 *
 * \return statusErrDef that values:
 * - errReadCANTC when CAN frame can't be read,
 * - noError when the function exits successfully.
 */
statusErrDef sendTelemToOBDH(const statusErrDef statusErr) {
	statusErrDef ret = noError;
	std::vector<uint8_t> telemOut;
	uint8_t categoryHighByte = (statusErr >> 8) & 0xFF;  // Get the higher byte (8 most significant bits)
    uint8_t categoryLowByte = statusErr & 0xFF; // Get the lower byte (8 least significant bits)

    telemOut = {categoryHighByte,categoryLowByte};

	std::vector<uint8_t> ccsdsPacket = generateCCSDSPacket(telemOut);
	if (ccsdsPacket.size() > DATA_OUT_CAN_MAX_LENGTH) {
        std::cerr << "Error: CCSDS packet too large for single CAN FD frame\n";
        return errCCSDSPacketTooLarge;
    }

    struct canfd_frame frame;
    frame.can_id = CAN_ID_OBDH;  // Set appropriate CAN ID
    frame.len = ccsdsPacket.size();  // Payload length

    std::memcpy(frame.data, ccsdsPacket.data(), ccsdsPacket.size());  // Copy CCSDS packet into frame

    if (write(socket_can, &frame, sizeof(struct canfd_frame)) != sizeof(struct canfd_frame)) {
        perror("CAN FD send error");
		return errWriteCANPayload;
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
statusErrDef recieveTCFromOBDH() {
	statusErrDef ret = noError;
	struct canfd_frame frame;

    if (read(socket_can, &frame, sizeof(struct canfd_frame)) < 0) {
    }

	ssize_t sizeReceived = read(socket_can, &frame, sizeof(struct canfd_frame));
	if (sizeReceived > 0) {
		//constructs an empty instance
		CCSDSSpacePacket ccsdsPacket;
		//interpret an input data as a CCSDS SpacePacket
		try {
			// Attempt to interpret the packet
			ccsdsPacket.interpret(frame.data, sizeReceived);
		} catch (CCSDSSpacePacketException &e) {
			// Print the exception details to help debug
			std::cerr << "CCSDS Packet Error: " << e.toString() << std::endl;
			std::cerr << "Failed to interpret packet of length " << sizeReceived << std::endl;
			// Optionally, dump the buffer contents for inspection
			for (size_t i = 0; i < sizeReceived; i++) {
				std::cout << std::hex << (int)frame.data[i] << " ";
			}
			std::cout << std::endl;
		}

		std::vector<uint8_t> *userData = ccsdsPacket.getUserDataField();
		mainStateTC = ((*userData)[0] << 8) | (*userData)[1];

		if(mainStateTC == 0x1701)
			resetMsgTimer();

		//get APID
		std::cout << ccsdsPacket.getPrimaryHeader()->getAPIDAsInteger() << std::endl;
		//dump packet content
		std::cout << ccsdsPacket.toString() << std::endl;
	} else {
        perror("errReadCANPayload");
        return errReadCANPayload;
	}
	return ret;
}

/**
 * \brief function to recieve telecommands from the OBDH subsystem.
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef checkTC() {
	counter++;
	statusErrDef ret = noError;
	ret = recieveTCFromOBDH();
	return ret;
}

void resetMsgTimer() {
	clock_gettime(CLOCK_MONOTONIC, &beginMsgTimer);
}

/**
 * \brief function to check if a 5G packet has
 * been recieved.
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef recieve5GPackets() {
	statusErrDef ret = noError;
	double currentTime = 0;
	ssize_t sizeReceived = 0;
	if(sizeReceived > 0) {
		clock_gettime(CLOCK_MONOTONIC, &beginMsgTimer);
		return info5GPacketReceived;
	}
	else {
		clock_gettime(CLOCK_MONOTONIC, &endMsgTimer);
		currentTime = (endMsgTimer.tv_sec - beginMsgTimer.tv_sec) +
			(endMsgTimer.tv_nsec - beginMsgTimer.tv_nsec) / 1e9;
	}
	if(currentTime >= MSG_TIMEOUT)
		ret = infoRecieve5GPacketsTimeout;
	return ret;
}

/**
 * \brief function to check if a navigation
 * request has been recieved.
 *
 * \return statusErrDef that values:
 * - noError when the function exits successfully.
 */
statusErrDef recieveNavReq() {
	statusErrDef ret = noError;
	return ret;
}
