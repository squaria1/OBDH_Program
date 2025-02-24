/**
 * \file safeMode.cpp
 * \brief Safe mode functions
 * \author Mael Parot
 * \version 1.0
 * \date 16/02/2025
 *
 * Safe mode functions
 *
 */
#include "safeMode.h"
#include "controlMode.h"
#include "init.h"


statusErrDef stopPayload() {
    statusErrDef ret = noError;
    std::vector<uint8_t> TCOut = {0x17, 0xFF};
	std::vector<uint8_t> ccsdsPacket = generateCCSDSPacket(TCOut);
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

statusErrDef broadcastSafeMode() {
    statusErrDef ret = noError;

    return ret;
}
