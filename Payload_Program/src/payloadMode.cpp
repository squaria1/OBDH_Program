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

//------------------------------------------------------------------------------
// Local function definitions
//------------------------------------------------------------------------------
std::vector<uint8_t> generateCCSDSPacket(std::vector<uint8_t> dataOut);
statusErrDef sendSensorDataToOBDH(const sensorDef sensorId, int32_t sensorValue);
statusErrDef recieveTCFromOBDH();

//------------------------------------------------------------------------------
// Global vars initialisation
//------------------------------------------------------------------------------
/**
 * \brief Determine if low power is activated or not.
 */
uint8_t counter = 0;
/**
 * \brief Determine if low power is activated or not.
 */
uint16_t mainStateTC = 0xFFFF;

/**
 * \brief beginning references of the message timeout timer.
 */
struct timespec beginMsgTimer;
/**
 * \brief ending references of the message timeout timer.
 */
struct timespec endMsgTimer;

//------------------------------------------------------------------------------
// Local function definitions
//------------------------------------------------------------------------------
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
	//set counters
	ccsdsPacketIN.getPrimaryHeader()->setSequenceCount(sequenceCount);
	//set data
	ccsdsPacketIN.setUserDataField(dataOut);
	ccsdsPacketIN.setPacketDataLength();
	//get packet as byte array
	std::vector<uint8_t> packet = ccsdsPacketIN.getAsByteVector();

	return packet;
}


/**
 * \brief function to send payload sensor data to
 * the OBDH subsystem via CAN bus.
 *
 * \return statusErrDef that values:
 * - errWriteCANPayload when the CAN frame can't be written,
 * - noError when the function exits successfully.
 */
statusErrDef sendSensorDataToOBDH(const sensorDef sensorId, int32_t sensorValue) {
    statusErrDef ret = noError;

    // Split sensorId into two bytes
    uint8_t SensorIdHighByte = (sensorId >> 8) & 0xFF;  // High byte of sensorId
    uint8_t SensorIdLowByte = sensorId & 0xFF;         // Low byte of sensorId

    // Split sensorValue (32-bit) into four bytes
    uint8_t SensorValueByte3 = (sensorValue >> 24) & 0xFF;  // Most significant byte
    uint8_t SensorValueByte2 = (sensorValue >> 16) & 0xFF;
    uint8_t SensorValueByte1 = (sensorValue >> 8) & 0xFF;
    uint8_t SensorValueByte0 = sensorValue & 0xFF;         // Least significant byte

    // SENSOR_DATA_SIZE is 7 (1 header + 2 for ID + 4 for value)
    uint8_t data[SENSOR_DATA_SIZE] = {
        0xFF,              // Header byte
        SensorIdHighByte,  // Sensor ID high byte
        SensorIdLowByte,   // Sensor ID low byte
        SensorValueByte3,  // Sensor value most significant byte
        SensorValueByte2,
        SensorValueByte1,
        SensorValueByte0   // Sensor value least significant byte
    };

	// Debug output
	/*
	for(int i = 0; i < 7; i++) {
		printf(" data[%d] : 0x%02X |",i,data[i]);
	}
	printf("\n");
	*/

	struct can_frame frame;  // Use classic CAN frame
    frame.can_id = CAN_ID_OBDH;
    frame.can_dlc = SENSOR_DATA_SIZE;  // Data length code (0-8 bytes)
    memcpy(frame.data, data, SENSOR_DATA_SIZE);

    // Write the frame (use sizeof(struct can_frame))
    if (write(socket_can, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("errWriteCANPayload");
        return errWriteCANPayload;
    }

	return ret;
}


/**
 * \brief function to send payload telemetry to the OBDH
 *
 * \return statusErrDef that values:
 * - errCCSDSPacketTooLarge When the CCSDS packet is too large for a CAN frame
 * - errWriteCANPayload when the CAN frame can't be written,
 * - noError when the function exits successfully.
 */
statusErrDef sendTelemToOBDH(const statusErrDef statusErr) {
    statusErrDef ret = noError;
    std::vector<uint8_t> telemOut;
    uint8_t categoryHighByte = (statusErr >> 8) & 0xFF;  // High byte
    uint8_t categoryLowByte = statusErr & 0xFF;          // Low byte
    telemOut = {categoryHighByte, categoryLowByte};

    std::vector<uint8_t> ccsdsPacket = generateCCSDSPacket(telemOut);
    if (ccsdsPacket.size() > DATA_OUT_CAN_MAX_LENGTH) {  // Classic CAN max payload is 8 bytes
        std::cerr << "Error: CCSDS packet too large for CAN frame\n";
        return errCCSDSPacketTooLarge;
    }

    struct can_frame frame;  // Use classic CAN frame
    frame.can_id = CAN_ID_OBDH;
    frame.can_dlc = ccsdsPacket.size();  // Data length code (0-8 bytes)
    std::memcpy(frame.data, ccsdsPacket.data(), ccsdsPacket.size());

    // Debug output
    /*
    std::cout << "CAN ID: " << frame.can_id << ", Length: " << (int)frame.can_dlc << std::endl;
    for (size_t i = 0; i < frame.can_dlc; i++) {
        std::cout << "Data[" << i << "]: " << (int)frame.data[i] << std::endl;
    }
    */

    // Write the frame (use sizeof(struct can_frame))
    if (write(socket_can, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
        perror("errWriteCANPayload");
        return errWriteCANPayload;
    }

    return ret;
}

/**
 * \brief function to recieve telecommands from the TT&C subsystem
 *
 * \return statusErrDef that values:
 * - errReadCANTC when CAN frame can't be read
 * - infoNoDataInCANBuffer when the read CAN function
 * returns EAGAIN or EWOULDBLOCK when there is no data
 * in the CAN buffer, we ignore it
 * - noError when the function exits successfully.
 */
statusErrDef recieveTCFromOBDH() {
	statusErrDef ret = noError;
    struct can_frame frame;  // Use classic CAN frame

	ssize_t sizeReceived = read(socket_can, &frame, sizeof(struct can_frame));
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

		if(mainStateTC == 0x1701) {
			resetMsgTimer();
            printf("State has been changed to control mode\n");
            sendTelemToOBDH(infoStateToPayloadMode);
		}

		//get APID
		std::cout << ccsdsPacket.getPrimaryHeader()->getAPIDAsInteger() << std::endl;
		//dump packet content
		std::cout << ccsdsPacket.toString() << std::endl;
	} else {
		// If there's no data, just continue (EAGAIN or EWOULDBLOCK)
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // No data available, continue the loop
            return infoNoDataInCANBuffer;
        } else {
			perror("errReadCANTC");
			return errReadCANTC;
		}
	}
	return ret;
}

/**
 * \brief function to recieve telecommands from the OBDH subsystem.
 *
 * \return statusErrDef that values:
 * - errReadCANTC when CAN frame can't be read
 * - infoNoDataInCANBuffer when the read CAN function
 * returns EAGAIN or EWOULDBLOCK when there is no data
 * in the CAN buffer, we ignore it
 * - noError when the function exits successfully.
 */
statusErrDef checkTC() {
	counter++;
	statusErrDef ret = noError;
	ret = recieveTCFromOBDH();
	return ret;
}

/**
 * \brief function to check the sensor values and send
 * them to the OBDH subsystem at every sampling time.
 *
 * \return statusErrDef that values:
 * - errWriteCANPayload when the CAN frame can't be written,
 * - noError when the function exits successfully.
 */
statusErrDef checkSensors() {
    statusErrDef ret = noError;
    static double nextSendTime = 2.0;  // First send at 2 seconds
    double currentTime;

    // Get current monotonic time
    clock_gettime(CLOCK_MONOTONIC, &endSensorSamplingTimer);

    // Calculate elapsed time since program start
    currentTime = (endSensorSamplingTimer.tv_sec - beginSensorSamplingTimer.tv_sec) +
                  (endSensorSamplingTimer.tv_nsec - beginSensorSamplingTimer.tv_nsec) / 1e9;

    // Check if we've reached or passed the next scheduled send time
    if (currentTime >= nextSendTime) {
		//printf("currentTime: %f\n",currentTime);
        int32_t sensorValue = counter;  // Get the current sensor value
        ret = sendSensorDataToOBDH(sensor1, sensorValue);

        // Schedule the next send exactly 2 seconds from the previous target
        nextSendTime += 2.0;
    }

    return ret;
}

/**
 * \brief function to reset the message timer when
 * a message has been recieved or when entering
 * payload mode.
 */
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
