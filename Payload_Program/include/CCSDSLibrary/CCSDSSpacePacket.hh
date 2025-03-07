/*
 * CCSDSSpacePacket.hh
 *
 *  Created on: Jun 9, 2011
 *      Author: yuasa
 */

#ifndef CCSDSSPACEPACKET_HH_
#define CCSDSSPACEPACKET_HH_

#include "CCSDSSpacePacketPrimaryHeader.hh"
#include "CCSDSSpacePacketSecondaryHeader.hh"
#include "CCSDSSpacePacketException.hh"
#include <vector>
#include <iomanip>
#include <sstream>

#if (defined(__GXX_EXPERIMENTAL_CXX0X) || (__cplusplus >= 201103L))
#include <cstdint>
#else
#include <stdint.h>
#endif

/** A class that represents a CCSDS SpacePacket.
 * This class contains structured information of a packet:
 * - Primary Header,
 * - Secondary Header (if present), and
 * - User Data Field.
 * .
 * Primary Header and Secondary Header are represented by dedicated classes
 * CCSDSSpacePacketPrimaryHeader and CCSDSSpacePacketSecondaryHeader, respectively.
 * User Data Field is implemented as std::vector<uint8_t>.
 *
 * @par
 * Example: Packet creation
 * @code
 CCSDSSpacePacket* ccsdsPacket = new CCSDSSpacePacket();
 ccsdsPacket->getPrimaryHeader()->setAPID(lowerAPID);
 ccsdsPacket->getPrimaryHeader()->setPacketType(CCSDSSpacePacketPacketType::TelemetryPacket);
 ccsdsPacket->getPrimaryHeader()->setSecondaryHeaderFlag(CCSDSSpacePacketSecondaryHeaderFlag::Present);
 ccsdsPacket->getPrimaryHeader()->setSequenceFlag(CCSDSSpacePacketSequenceFlag::UnsegmentedUserData);
 ccsdsPacket->getSecondaryHeader()->setCategory(category);
 ccsdsPacket->getSecondaryHeader()->
 setSecondaryHeaderType(CCSDSSpacePacketSecondaryHeaderType::ADUChannelIsUsed);
 ccsdsPacket->getSecondaryHeader()->setADUChannelID(0x00);
 ccsdsPacket->getSecondaryHeader()->setADUSegmentFlag(CCSDSSpacePacketADUSegmentFlag::UnsegmentedADU);
 //set counters
 ccsdsPacket->getPrimaryHeader()->setSequenceCount(sequenceCount);
 ccsdsPacket->getSecondaryHeader()->setADUCount(aduCount);

 //set absolute time
 uint8_t time[4];
 ccsdsPacket->getSecondaryHeader()->setTime(time);

 //set data
 ccsdsPacket->setUserDataField(smcpByteArray);
 ccsdsPacket->setPacketDataLength();

 //get packet as byte array
 std::vector<uint8_t> packet = ccsdsPacket->getAsByteVector();
 * @endcode
 *
 * Example: Packet interpretation
 * @code
 CCSDSSpacePacket* ccsdsPacket = new CCSDSSpacePacket();
 ccsdsPacket->interpret(data,length);
 std::cout << ccsdsPacket->toString() << std::endl;
 * @endcode
 *
 * @see CCSDSSpacePacketPrimaryHeader, CCSDSSpacePacketSecondaryHeader
 */
class CCSDSSpacePacket {
public:
	static const uint32_t APIDOfIdlePacket = 0x7FF; //111 1111 1111

public:
	CCSDSSpacePacketPrimaryHeader* primaryHeader;
	CCSDSSpacePacketSecondaryHeader* secondaryHeader;
	std::vector<uint8_t>* userDataField;

public:
	/** Constructs an instance.
	 * Internal instances of the primary header, the secondary header,
	 * and the user data field are constructed.
	 */
	CCSDSSpacePacket() {
		primaryHeader = new CCSDSSpacePacketPrimaryHeader();
		secondaryHeader = new CCSDSSpacePacketSecondaryHeader();
		userDataField = new std::vector<uint8_t>();
		//primaryHeader->setPacketVersionNum(CCSDSSpacePacketPacketVersionNumber::Version1);
	}

public:
	/** Destructor.
	 * Internal instances of the primary header, the secondary header,
	 * and the user data field are deleted.
	 */
	virtual ~CCSDSSpacePacket() {
		delete primaryHeader;
		delete secondaryHeader;
		delete userDataField;
	}

public:
	/** Copy constructor.
	 */
	CCSDSSpacePacket(const CCSDSSpacePacket& obj) {
		using namespace std;
		this->primaryHeader = new CCSDSSpacePacketPrimaryHeader();
		this->secondaryHeader = new CCSDSSpacePacketSecondaryHeader();
		this->userDataField = new std::vector<uint8_t>();

		*this->primaryHeader = *(obj.primaryHeader);
		*this->secondaryHeader = *(obj.secondaryHeader);
		*this->userDataField = *(obj.userDataField);
	}

public:
	/** Constructs and returns a new instance that has the same information as the current instance.
	 * @return a pointer of the newly created instance
	 */
	CCSDSSpacePacket* clone(){
		CCSDSSpacePacket* result=new CCSDSSpacePacket;
		*(result->primaryHeader) = *(this->primaryHeader);
		*(result->secondaryHeader) = *(this->secondaryHeader);
		*(result->userDataField) = *(this->userDataField);
		return result;
}

public:
	/** Returns packet content as a vector of uint8_t.
	 * Packet content will be dynamically generated every time
	 * when this method is invoked.
	 * @return a uint8_t vector that contains packet content
	 */
	std::vector<uint8_t> getAsByteVector() {
		calcPacketDataLength();
		std::vector<uint8_t> result;
		std::vector<uint8_t> primaryHeaderVector = primaryHeader->getAsByteVector();
		result.insert(result.end(), primaryHeaderVector.begin(), primaryHeaderVector.end());
		if (primaryHeader->getSecondaryHeaderFlag().to_ulong() == CCSDSSpacePacketSecondaryHeaderFlag::Present) {
			std::vector<uint8_t> secondaryHeaderVector = secondaryHeader->getAsByteVector();
			result.insert(result.end(), secondaryHeaderVector.begin(), secondaryHeaderVector.end());
		}
		result.insert(result.end(), userDataField->begin(), userDataField->end());
		return result;
	}

public:
	/** Interprets a uint8_t array into this instance.
	 * When the array does not contain a valid CCSDS SpacePacket, an exception may
	 * be thrown.
	 * @param[in] buffer a pointer to a uint8_t array that contains a CCSDS SpacePacket.
	 * @param[in] length the length of the data contained in buffer.
	 */
	void interpret(const uint8_t *buffer, size_t length) {
		using namespace std;

		if (length < 6) {
			throw CCSDSSpacePacketException(CCSDSSpacePacketException::NotACCSDSSpacePacket);
		}
		//primary header
		primaryHeader->interpret(buffer);
		size_t packetDataLengthCorrected1 = primaryHeader->getPacketDataLength() + 1;
		size_t totalPacketLength = packetDataLengthCorrected1 + CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength;

		if (length < totalPacketLength) {
			throw CCSDSSpacePacketException(CCSDSSpacePacketException::InconsistentPacketLength);
		}

		if (primaryHeader->getSecondaryHeaderFlag().to_ulong() == CCSDSSpacePacketSecondaryHeaderFlag::NotPresent) {
			//buffer field
			userDataField->clear();
			for (size_t i = CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength; i < totalPacketLength; i++) {
				userDataField->push_back(buffer[i]);
			}
		} else {
			//secondary header
			secondaryHeader->interpret(buffer + CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength, //
			length - CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength);
			//buffer field
			userDataField->clear();
			for (size_t i = CCSDSSpacePacketPrimaryHeader::PrimaryHeaderLength + secondaryHeader->getLength();
					i < totalPacketLength; i++) {
				userDataField->push_back(buffer[i]);
			}
		}

	}

public:
	/** Interprets data contained in a uint8_t vector into this instance.
	 * @param[in] buffer a uint8_t vector that contains a CCSDS SpacePacket.
	 */
	void interpret(const std::vector<uint8_t> & buffer) {
		if (buffer.size() != 0) {
			interpret(&(buffer[0]), buffer.size());
		}
	}

public:
	/** Interprets data contained in a uint8_t vector into this instance.
	 * @param[in] buffer a pointer to a uint8_t vector that contains a CCSDS SpacePacket.
	 */
	void interpret(const std::vector<uint8_t>* buffer) {
		if (buffer->size() != 0) {
			interpret(&((*buffer)[0]), buffer->size());
		}
	}

private:
	void calcPacketDataLength() {
		if (primaryHeader->getSecondaryHeaderFlag().to_ulong() == CCSDSSpacePacketSecondaryHeaderFlag::Present) {
			primaryHeader->setPacketDataLength(secondaryHeader->getAsByteVector().size() + userDataField->size() - 1);
		} else {
			primaryHeader->setPacketDataLength(userDataField->size() - 1);
		}
	}

public:
	/** Fill Packet Data Length field with an appropriate number
	 * which is automatically calculated by this class.
	 */
	void setPacketDataLength() {
		calcPacketDataLength();
	}

public:
	/** Sets uint8_t array to User Data Field.
	 */
	void setUserDataField(const std::vector<uint8_t> *userDataField) {
		*(this->userDataField) = *userDataField;
		setPacketDataLength();
	}

public:
	/** Sets uint8_t array to User Data Field.
	 */
	void setUserDataField(const std::vector<uint8_t> userDataField) {
		*(this->userDataField) = userDataField;
		setPacketDataLength();
	}

public:
	/** Returns string dump of this packet.
	 * @returns string dump of this packet.
	 */
	virtual std::string toString() {
		const size_t maxBytesToBeDumped = 32;
		std::stringstream ss;
		using std::endl;
		ss << "---------------------------------" << endl;
		ss << "CCSDSSpacePacket" << endl;
		ss << "---------------------------------" << endl;
		ss << primaryHeader->toString();
		if (primaryHeader->getSecondaryHeaderFlag().to_ulong() == CCSDSSpacePacketSecondaryHeaderFlag::Present) {
			ss << secondaryHeader->toString();
		} else {
			ss << "No secondary header" << endl;
		}
		if (userDataField->size() != 0) {
			ss << "User data field has " << std::dec << userDataField->size();
			if (userDataField->size() < 2) {
				ss << " byte" << endl;
			} else {
				ss << " bytes" << endl;
			}
			ss << arrayToString(userDataField, "hex", maxBytesToBeDumped) << endl;
		} else {
			ss << "No user data field" << endl;
		}
		ss << endl;
		return ss.str();
	}

public:
	/** Dumps self as string to a provided output stream.
	 * @param[in] os output stream such as fstream or cout.
	 */
	virtual void dump(std::ostream& os) {
		os << this->toString();
	}

public:
	/** Dumps self as string to a provided output stream.
	 * @param[in] os output stream such as fstream or cout.
	 */
	virtual void dump(std::ostream* os) {
		*os << this->toString();
	}

public:
	/** Dumps self as string to the screen.
	 */
	virtual void dumpToScreen() {
		dump(std::cout);
	}

public:
	/** A utility method which converts std::vector<uint8_t> to
	 * std::string.
	 */
	static std::string arrayToString(std::vector<uint8_t> *data, std::string mode = "dec", size_t maxBytesToBeDumped = 8) {
		//copied from CxxUtilities::Array
		using namespace std;

		stringstream ss;
		size_t maxSize;
		if (data->size() < maxBytesToBeDumped) {
			maxSize = data->size();
		} else {
			maxSize = maxBytesToBeDumped;
		}
		for (size_t i = 0; i < maxSize; i++) {
			if (mode == "dec") {
				ss << dec << left << (uint32_t) (data->at(i));
			} else if (mode == "hex") {
				ss << hex << "0x" << setw(2) << setfill('0') << right << (uint32_t) (data->at(i));
			} else {
				ss << data->at(i);
			}
			if (i != maxSize - 1) {
				ss << " ";
			}
		}
		ss << dec;
		if (maxSize < data->size()) {
			ss << " ... (total size = " << dec << data->size();
			if (data->size() == 1) {
				ss << " entry)";
			} else {
				ss << " entries)";
			}
		}
		return ss.str();
	}

public:
	/** Checks if Secondary Header is present.
	 */
	inline bool isSecondaryHeaderPresent() {
		if (primaryHeader->getSecondaryHeaderFlag().to_ulong() == CCSDSSpacePacketSecondaryHeaderFlag::Present) {
			return true;
		} else {
			return false;
		}
	}

public:
	/** Checks if Secondary Header is present.
	 */
	inline bool hasSecondaryHeader() {
		return isSecondaryHeaderPresent();
	}

public:
	/** Checks if Secondary Header is present.
	 */
	inline bool isSecondaryHeaderUsed() {
		return isSecondaryHeaderPresent();
	}

public:
	/** Returns a Primary Header instance.
	 * @returns a pointer to the Primary Header of this packet.
	 */
	inline CCSDSSpacePacketPrimaryHeader* getPrimaryHeader() const {
		return primaryHeader;
	}

public:
	/** Returns a Secondary Header instance.
	 * @returns a pointer to the Secondary Header of this packet.
	 */
	inline CCSDSSpacePacketSecondaryHeader* getSecondaryHeader() const {
		return secondaryHeader;
	}

public:
	/** Returns a User Data Field pointer (std::vector<uint8_t>*).
	 * @returns a pointer to the User Data Field of this packet.
	 */
	inline std::vector<uint8_t>* getUserDataField() {
		return userDataField;
	}

public:
	/** True if TC Packet.
	 */
	bool isTCPacket(){
		return (primaryHeader->getPacketType().to_ulong()==1)?true:false;
	}

public:
	/** True if TM Packet.
	 */
	bool isTMPacket(){
		return (primaryHeader->getPacketType().to_ulong()==0)?true:false;
	}

public:
	/** True if Packet is segmented.
	 */
	inline bool isSegmented() {
		return primaryHeader->isSegmented();
	}

public:
	/** True if Packet is the first segmented.
	 */
	inline bool isFirstSegment() {
		return primaryHeader->isFirstSegment();
	}

public:
	/** True if Packet is the last segmented.
	 */
	inline bool isLastSegment() {
		return primaryHeader->isLastSegment();
	}

public:
	/** True if Packet is a continuation segmented.
	 */
	inline bool isContinuationSegment() {
		return primaryHeader->isContinuationSegment();
	}

public:
	/** True if Packet is an unsegmented packet.
	 */
	inline bool isUnsegmented() {
		return primaryHeader->isUnsegmented();
	}

};

#endif /* CCSDSSPACEPACKET_HH_ */
