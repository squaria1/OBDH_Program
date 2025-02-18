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
#include "controlMode.h"
#include "init.h"

int toInteger(std::string str) {
	using namespace std;
	stringstream ss;
	ss << str;
	int avalue;
	if (str.size() >= 2 && str[0] == '0' && (str[1] == 'X' || str[1] == 'x')) {
		ss >> hex >> avalue;
	} else {
		ss >> avalue;
	}
	return avalue;
}

/**
 * \brief function to send telemetry to the TT&C subsystem
 *
 * \return statusErrDef that values:
 * - errWriteCANTelem when the CAN frame can't be written,
 * - noError when the function exits successfully.
 */
statusErrDef processCCSDSPacket() {
	statusErrDef ret = noError;
	using namespace std;
	uint16_t apid = 0;
	uint8_t category = 0;
	uint8_t aduCount = 0;
	size_t sequenceCount = 0;
	std::vector<uint8_t> dummyData = {0x01, 0x02, 0x03, 0x04, 0x05};


	//constructs an empty instance
	CCSDSSpacePacket* ccsdsPacketIN = new CCSDSSpacePacket();

	//set APID
	ccsdsPacketIN->getPrimaryHeader()->setAPID(apid);

	//set Packet Type (Telemetry or Command)
	ccsdsPacketIN->getPrimaryHeader()->setPacketType(CCSDSSpacePacketPacketType::TelemetryPacket);

	//set Secondary Header Flag (whether this packet has the Secondary Header part)
	ccsdsPacketIN->getPrimaryHeader()->setSecondaryHeaderFlag(CCSDSSpacePacketSecondaryHeaderFlag::Present);

	//set segmentation information
	ccsdsPacketIN->getPrimaryHeader()->setSequenceFlag(CCSDSSpacePacketSequenceFlag::UnsegmentedUserData);

	//set Category
	ccsdsPacketIN->getSecondaryHeader()->setCategory(category);

	//set secondary header type (whether ADU Channel presence)
	ccsdsPacketIN->getSecondaryHeader()->
	setSecondaryHeaderType(CCSDSSpacePacketSecondaryHeaderType::ADUChannelIsUsed);

	//set ADU Channel ID
	ccsdsPacketIN->getSecondaryHeader()->setADUChannelID(0x00);

	//set ADU Segmentation Flag (whether ADU is segmented)
	ccsdsPacketIN->getSecondaryHeader()->setADUSegmentFlag(CCSDSSpacePacketADUSegmentFlag::UnsegmentedADU);

	//set counters
	ccsdsPacketIN->getPrimaryHeader()->setSequenceCount(sequenceCount);
	ccsdsPacketIN->getSecondaryHeader()->setADUCount(aduCount);

	//set absolute time
	uint8_t time[4];
	ccsdsPacketIN->getSecondaryHeader()->setTime(time);

	//set data
	ccsdsPacketIN->setUserDataField(dummyData);
	ccsdsPacketIN->setPacketDataLength();

	//get packet as byte array
	std::vector<uint8_t> packet = ccsdsPacketIN->getAsByteVector();

	printf("\n\n\n==================================================\n\n\n");

	//constructs an empty instance
	CCSDSSpacePacket* ccsdsPacket = new CCSDSSpacePacket();

	//interpret an input data as a CCSDS SpacePacket
	ccsdsPacket->interpret(packet.data(),packet.size());

	//check if the packet has Secondary Header
	if(ccsdsPacket->isSecondaryHeaderPresent()){
	}

	//get APID
	std::cout << ccsdsPacket->getPrimaryHeader()->getAPIDAsInteger() << std::endl;

	//dump packet content
	std::cout << ccsdsPacket->toString() << std::endl;

	return ret;
}

statusErrDef sendTelemToTTC(const statusErrDef statusErr) {
	statusErrDef ret = noError;
	struct can_frame frame;

	printf("sendTelemToTTC test \r\n");

	frame.can_id = 0x555;
	frame.can_dlc = 5;
	memcpy(frame.data, "Hello", frame.can_dlc);

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
	int s = 0, i;
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

statusErrDef checkSensors() {
	statusErrDef ret = noError;
	ret = processCCSDSPacket();
	return ret;
}

statusErrDef checkTC() {
	statusErrDef ret = noError;
	return ret;
}
