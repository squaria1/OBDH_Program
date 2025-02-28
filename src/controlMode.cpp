/**
 * \file controlMode.cpp
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

uint8_t counter = 0;
uint16_t mainStateTC = 0xFFFF;

/**
 * \brief function to generate a CCSDS packet wrapping user data
 *
 * \param dataOut the data to recieve from a UDP or CAN frame,
 * cut in a vector of bytes
 *
 * \return packet a CCSDS packet to be sent
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
	if(ccsdsPacket->isSecondaryHeaderPresent()){
		printf("HAS A SECONDARY HEADER\n");
	}
	//get APID
	std::cout << ccsdsPacket->getPrimaryHeader()->getAPIDAsInteger() << std::endl;
	//dump packet content
	std::cout << ccsdsPacket->toString() << std::endl;
	*/

	return packet;
}

/**
 * \brief function to send telemetry to the TT&C subsystem
 *
 * \param statusErr the telemetry data (see statesDefine.h)
 *
 * \return statusErrDef that values:
 * - errWriteUDPTelem when the telemetry can't be sent,
 * - noError when the function exits successfully.
 */
statusErrDef sendTelemToTTC(const statusErrDef statusErr) {
	statusErrDef ret = noError;
	std::vector<uint8_t> telemOut;
	uint8_t categoryHighByte = (statusErr >> 8) & 0xFF;  // Get the higher byte (8 most significant bits)
    uint8_t categoryLowByte = statusErr & 0xFF; // Get the lower byte (8 least significant bits)

    telemOut = {categoryHighByte,categoryLowByte};

	std::vector<uint8_t> ccsdsPacket = generateCCSDSPacket(telemOut);

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

    //std::cout << "Sent " << bytes_sent << " bytes over UDP\n";

	return ret;
}

/**
 * \brief function to send telemetry to the TT&C subsystem
 *
 * \param sensor the sensor ID (see statesDefine.h)
 * \param sensorValue the sensor value in a series of bytes
 *
 * \return statusErrDef that values:
 * - errWriteUDPTelem when the telemetry can't be sent,
 * - noError when the function exits successfully.
 */
statusErrDef sendSensorDataToTTC(const sensorDef sensor, std::vector<uint8_t> sensorValue) {
	statusErrDef ret = noError;
	std::vector<uint8_t> telemOut;
	uint8_t categoryHighByte = (sensor >> 8) & 0xFF;  // Get the higher byte (8 most significant bits)
    uint8_t categoryLowByte = sensor & 0xFF; // Get the lower byte (8 least significant bits)

	telemOut = {categoryHighByte,categoryLowByte};
	for(int i = 0; i < sensorValue.size(); i++) {
		telemOut.push_back(sensorValue[i]);
	}

	std::vector<uint8_t> ccsdsPacket = generateCCSDSPacket(telemOut);

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

    //std::cout << "Sent " << bytes_sent << " bytes over UDP\n";

	return ret;
}

void DumpUDPData(uint8_t *data, ssize_t length) {
    printf("Received %zd bytes of data:\n", length);

    // Iterate over each byte in the received data
    for (ssize_t i = 0; i < length; i++) {
        // Print each byte in hexadecimal format
        printf("0x%02X ", data[i]);

        // Optionally, print a newline every 16 bytes for better readability
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }

    // Print a final newline if the data doesn't end on a boundary of 16 bytes
    if (length % 16 != 0) {
        printf("\n");
    }
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
	uint16_t mainStateTCRecieved;
	uint16_t mostSigHexDigitTC;
	struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    uint8_t buffer[UDP_MAX_BUFFER_SIZE];

	ssize_t sizeReceived = recvfrom(socket_udp, buffer, UDP_MAX_BUFFER_SIZE, 0,(struct sockaddr*)&clientAddr,&addrLen);
	if (sizeReceived > 0) {
		std::cout << "Received " << sizeReceived << " bytes from cFS\n";
		DumpUDPData(buffer, sizeReceived);
		//constructs an empty instance
		CCSDSSpacePacket ccsdsPacket;
		//interpret an input data as a CCSDS SpacePacket
		try {
			// Attempt to interpret the packet
			ccsdsPacket.interpret(buffer, sizeReceived);
		} catch (CCSDSSpacePacketException &e) {
			// Print the exception details to help debug
			std::cerr << "CCSDS Packet Error: " << e.toString() << std::endl;
			std::cerr << "Failed to interpret packet of length " << sizeReceived << std::endl;
			// Optionally, dump the buffer contents for inspection
			for (size_t i = 0; i < sizeReceived; i++) {
				std::cout << std::hex << (int)buffer[i] << " ";
			}
			std::cout << std::endl;

			return errCCSDSPacketUninterpretable;
		}

		std::vector<uint8_t> *userData = ccsdsPacket.getUserDataField();

		mainStateTCRecieved = ((*userData)[0] << 8) | (*userData)[1];
		mostSigHexDigitTC = mainStateTCRecieved & 0xF000;

		switch(mostSigHexDigitTC) {
			case OBDHSubsystem:
				mainStateTC = mainStateTCRecieved;
				break;
			case payloadSubsystem:
				ret = sendTCToSubsystem(*userData, payloadSubsystem);
				break;
			case everySubsystems:
				mainStateTC = mainStateTCRecieved & 0x0FFF;
				ret = sendTCToSubsystem(*userData, everySubsystems);
				break;
			default:
				return errTCToWrongSubsystem;
				break;
		}

		//get APID
		std::cout << ccsdsPacket.getPrimaryHeader()->getAPIDAsInteger() << std::endl;
		//dump packet content
		std::cout << ccsdsPacket.toString() << std::endl;
	}
	return ret;
}

/**
 * \brief function to recieve telemetry from all subsystems
 *
 * \return statusErrDef that values:
 * - errReadCANTelem when CAN frame can't be read from the Payload subsystem,
 * - noError when the function exits successfully.
 */
statusErrDef recieveTelemFromSubsystems() {
	statusErrDef ret = noError;
	struct canfd_frame frame;

    if (read(socket_can, &frame, sizeof(struct canfd_frame)) < 0) {
        perror("errReadCANTelem");
        return errReadCANTelem;
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
 * \brief function to send telecommands to the Payload subsystem.
 *
 * \param TCOut the telecommands to transmit to a CAN frame to
 * the payload subsystem, cut in a vector of bytes.
 *
 * \return statusErrDef that values:
 * - errCCSDSPacketTooLarge when the CCSDS packet is too large for the CAN FD frame (64 Bytes),
 * - errWriteCANPayload when write payload subsystem TCs to the CAN bus fails,
 * - noError when the function exits successfully.
 */
statusErrDef sendTCToSubsystem(std::vector<uint8_t> TCOut, subsystemDef subsystem) {
	statusErrDef ret = noError;
	canid_t canId = 0x000;

	switch(subsystem) {
		case payloadSubsystem:
				canId = CAN_ID_PAYLOAD;
			break;
		case everySubsystems:
				canId = CAN_ID_BROADCAST;
		default:
			return errTCToWrongSubsystem;
			break;
	}

	std::vector<uint8_t> ccsdsPacket = generateCCSDSPacket(TCOut);
	if (ccsdsPacket.size() > DATA_OUT_CAN_MAX_LENGTH) {
        std::cerr << "Error: CCSDS packet too large for single CAN FD frame\n";
        return errCCSDSPacketTooLarge;
    }

    struct canfd_frame frame;
    frame.can_id = canId;  // Set appropriate CAN ID
    frame.len = ccsdsPacket.size();  // Payload length

    std::memcpy(frame.data, ccsdsPacket.data(), ccsdsPacket.size());  // Copy CCSDS packet into frame

    if (write(socket_can, &frame, sizeof(struct canfd_frame)) != sizeof(struct canfd_frame)) {
        perror("CAN FD send error");
		return errWriteCANTC;
    }
    else {
		std::cout << "Sent CCSDS packet (" << ccsdsPacket.size() << " bytes) in a single CAN FD frame\n";
    }

	return ret;
}

/**
 * \brief function to compare every sensor current values with the warning
 * and critical bounds declared in the paramSensors.csv file.
 *
 * \return statusErrDef that values:
 * - errSensorCriticalValue when a sensor has reached a minimum or maximum warning value from the paramSensors.csv file.
 * - errSensorWarningValue when a sensor has reached a minimum or maximum critical value from the paramSensors.csv file.
 * - noError when the function exits successfully.
 */
statusErrDef compareSensorValuesWithParam() {
	statusErrDef ret = noError;
	for(int i = 1; i < lineCountSensorParamCSV; i++) {
		// Check if the sensor current value is out of its warning bounds
		if(paramSensors->currentValue[i] <= paramSensors->minWarnValue[i] ||
			paramSensors->currentValue[i] >= paramSensors->maxWarnValue[i]) {
				// Check if the sensor current value is out of its critical bounds
				if(paramSensors->currentValue[i] <= paramSensors->minCriticalValue[i] ||
				paramSensors->currentValue[i] >= paramSensors->maxCriticalValue[i]) {
					sendTelemToTTC(errSensorCriticalValue);
					return errSensorCriticalValue;
				}
				else {
					sendTelemToTTC(errSensorWarningValue);
					return errSensorWarningValue;
				}
			}
	}

	return ret;
}

/**
 * \brief function to recieve sensor telemetry data from
 * every spacecraft subsystems and check if their values
 * are out of bounds.
 *
 * \return statusErrDef that values:
 * - errReadCANEPS when CAN frame can't be read from the EPS subsystem,
 * - noError when the function exits successfully.
 */
statusErrDef checkSensors() {
	statusErrDef ret = noError;
	ret = recieveTelemFromSubsystems();
	if(ret != noError)
		return ret;
	ret = compareSensorValuesWithParam();
	return ret;
}

/**
 * \brief function to recieve telecommands from the TT&C subsystem
 * and redirect sensor data as telemetry to the TT&C subsystem.
 *
 * \return statusErrDef that values:
 * - errReadCANTC when CAN frame can't be read,
 * - errWriteUDPTelem when the telemetry can't be sent,
 * - noError when the function exits successfully.
 */
statusErrDef checkTC() {
	counter++;
	statusErrDef ret = noError;
	ret = recieveTCFromTTC();
	if(ret != noError)
		return ret;
	ret = sendSensorDataToTTC(sensor1, {counter});
	if(ret != noError)
		return ret;
	ret = sendSensorDataToTTC(sensor2, {0xF1, counter});
	if(ret != noError)
		return ret;
	ret = sendSensorDataToTTC(sensor3, {0xF1, 0xF2, 0xF3, counter});
	return ret;
}
