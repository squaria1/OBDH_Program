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
 * \return a CCSDS packet to be sent
 */
std::vector<uint8_t> generateCCSDSPacket(std::vector<uint8_t> dataOut, busTypeDef busType) {
	using namespace std;
	uint16_t apid = 0x80b;
	uint8_t category = 0;
	uint8_t aduCount = 0;
	size_t sequenceCount = 0;

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
	ccsdsPacketIN->setUserDataField(dataOut);
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
		printf("HAS A SECONDARY HEADER\n");
	}
	//get APID
	std::cout << ccsdsPacket->getPrimaryHeader()->getAPIDAsInteger() << std::endl;
	//dump packet content
	std::cout << ccsdsPacket->toString() << std::endl;

	return packet;
}


statusErrDef sendTelemToTTC(const statusErrDef statusErr) {
	statusErrDef ret = noError;
	busTypeDef busType = UDPType;
	uint8_t highByte = (statusErr >> 8) & 0xFF;  // Get the higher byte (8 most significant bits)
    uint8_t lowByte = statusErr & 0xFF; // Get the lower byte (8 least significant bits)

	std::vector<uint8_t> telemOut = {highByte,lowByte};
	std::vector<uint8_t> ccsdsPacket = generateCCSDSPacket(telemOut, busType);

	// Setup the destination address (this is where the packet will be sent)
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_port = htons(UDP_TELEMETRY_PORT);  // Destination port
    clientAddr.sin_addr.s_addr = inet_addr(TTC_IP_ADDRESS);  // Destination IP address (localhost, change to actual IP)

    // Send the CCSDS packet over UDP
    ssize_t bytes_sent = sendto(socket_udp, ccsdsPacket.data(), ccsdsPacket.size(),
                                 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
    if (bytes_sent < 0) {
        perror("errWriteUDPTelem");
        return errWriteUDPTelem;  // Error in sending packet
    }

    std::cout << "Sent " << bytes_sent << " bytes over UDP\n";

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
	struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    uint8_t buffer[UDP_MAX_BUFFER_SIZE];

	ssize_t sizeReceived = recvfrom(socket_udp, buffer, UDP_MAX_BUFFER_SIZE, 0,(struct sockaddr*)&clientAddr,&addrLen);
	if (sizeReceived > 0) {
		std::cout << "Received " << sizeReceived << " bytes from cFS\n";
		//constructs an empty instance
		CCSDSSpacePacket* ccsdsPacket = new CCSDSSpacePacket();
		//interpret an input data as a CCSDS SpacePacket
		ccsdsPacket->interpret(buffer,sizeReceived);
		//check if the packet has Secondary Header
		if(ccsdsPacket->isSecondaryHeaderPresent()){
			printf("HAS A SECONDARY HEADER\n");
		}
		//get APID
		std::cout << ccsdsPacket->getPrimaryHeader()->getAPIDAsInteger() << std::endl;
		//dump packet content
		std::cout << ccsdsPacket->toString() << std::endl;
	}
	return ret;
}

/**
 * \brief function to recieve telemetry from the Payload subsystem
 *
 * \return statusErrDef that values:
 * - errReadCANPayload when CAN frame can't be read,
 * - noError when the function exits successfully.
 */
statusErrDef recieveTelemFromPayload() {
	statusErrDef ret = noError;
	struct canfd_frame frame;

    if (read(socket_fd, &frame, sizeof(struct canfd_frame)) < 0) {
        perror("errReadCANPayload");
        return errReadCANPayload;
    }

    std::cout << "Received CCSDS packet (" << static_cast<int>(frame.len) << " bytes)\n";
    std::cout << "Data: ";
    for (size_t i = 0; i < frame.len; i++) {
        printf("%02X ", frame.data[i]);
    }
    std::cout << std::endl;

	return ret;
}

/**
 * \brief function to send telecommands to the Payload subsystem
 *
 * \return statusErrDef that values:
 * - errWriteCANPayload when CAN frame can't be written,
 * - noError when the function exits successfully.
 */
statusErrDef sendTCToPayload(std::vector<uint8_t> TCOut) {
	statusErrDef ret = noError;
	busTypeDef busType = CANType;
	std::vector<uint8_t> ccsdsPacket = generateCCSDSPacket(TCOut, busType);
	if (ccsdsPacket.size() > DATA_OUT_CAN_MAX_LENGTH) {
        std::cerr << "Error: CCSDS packet too large for single CAN FD frame\n";
        return errCCSDSPacketTooLarge;
    }

    struct canfd_frame frame;
    frame.can_id = CAN_ID_OBDH;  // Set appropriate CAN ID
    frame.len = ccsdsPacket.size();  // Payload length

    std::memcpy(frame.data, ccsdsPacket.data(), ccsdsPacket.size());  // Copy CCSDS packet into frame

    if (write(socket_fd, &frame, sizeof(struct canfd_frame)) != sizeof(struct canfd_frame)) {
        perror("CAN FD send error");
		return errWriteCANPayload;
    }
    else {
		std::cout << "Sent CCSDS packet (" << ccsdsPacket.size() << " bytes) in a single CAN FD frame\n";
    }

	return ret;
}

statusErrDef checkSensors() {
	statusErrDef ret = noError;
	ret = recieveTelemFromPayload();
	return ret;
}

statusErrDef checkTC() {
	statusErrDef ret = noError;
	ret = recieveTCFromTTC();
	return ret;
}
