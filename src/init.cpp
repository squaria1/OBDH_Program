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


//------------------------------------------------------------------------------
// Local function definitions
//------------------------------------------------------------------------------
statusErrDef initSensorParamCSV();
int countFileLines(const char *filename);
statusErrDef readParamSensorsFile(const char* fileName);
void fillParamSensorsStruct(char* line, int pos);
statusErrDef initCANSocket();
statusErrDef initUDPSocket();

/**
 * \struct paramSensors
 * \brief struct containing the parameters of each sensors
 * on the spacecraft
 *
 */
struct paramSensorsStruct* paramSensors;

/**
 * \brief the CAN socket global variable.
 */
int socket_can = 0;

/**
 * \brief the UDP socket global variable.
 */
int socket_udp = 0;

/**
 * \brief paramSensors.csv file line count.
 */
int lineCountSensorParamCSV = 0;

//------------------------------------------------------------------------------
// Local function definitions
//------------------------------------------------------------------------------
/**
 * \brief function to read "paramSensors.csv"
 * until the end of the file.
 *
 * \return statusErrDef that values:
 * - errOpenParamSensorsFile when the paramSensors.csv file fails to open
 * - errAllocParamSensorStruct when the sensorParam structure cannot be allocated to the memory
 * - noError when the function exits successfully.
 */
statusErrDef initSensorParamCSV() {
	statusErrDef ret = noError;
	lineCountSensorParamCSV = countFileLines(PARAM_SENSORS_CSV_FILEPATH);
    if(lineCountSensorParamCSV == -1) {
		return errOpenParamSensorsFile;
    }

	paramSensors = (struct paramSensorsStruct*)malloc(sizeof(struct paramSensorsStruct));
	if (paramSensors == NULL)
	{
		perror("Error allocating memory");
		return errAllocParamSensorStruct;
	}

	memset(paramSensors, 0, sizeof(struct paramSensorsStruct));

	ret = readParamSensorsFile(PARAM_SENSORS_CSV_FILEPATH);

	return ret;
}

/**
 * \brief function to count the number of lines
 * in the paraSensors.csv file.
 *
 * \param filename location and name of the CSV file to read
 *
 * \return the line count.
 */
int countFileLines(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("File open error");
        return -1;
    }

    int count = 0;
    char ch;

    while ((ch = fgetc(file)) != EOF) {
        if (ch == '\n') {
            count++;
        }
    }

    fclose(file);
    return count;
}

/**
 * \brief function to read "paramSensors.csv"
 * until the end of the file.
 *
 * \param fileName location and name of the CSV file to read
 * \return statusErrDef that values:
 * - errOpenParamSensorsFile when the paramSensors.csv file fails to open
 * - noError when the function exits successfully.
 */
statusErrDef readParamSensorsFile(const char* fileName) {
    FILE* file = fopen(fileName, "r");
    if (file == NULL)
    {
        perror("File open error");
        return errOpenParamSensorsFile;
    }
    int pos = 0;

    char line[MAX_CSV_LINE_SIZE];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        line[strcspn(line, "\n")] = 0;
        fillParamSensorsStruct(line, pos);
        pos++;
    }

    fclose(file);
    return noError;
}

/**
 * \brief function to fill in the sensors parameters structure.
 *
 * \param line the CSV line to read.
 * \param pos the position of the line in the CSV file.
 */
void fillParamSensorsStruct(char* line, int pos) {
    char* token = strtok(line, ";");
    int column = 0;

    while (token != NULL)
    {
        switch (column)
        {
        case 1:
            paramSensors->id[pos] = (uint16_t)strtol(token, NULL, 0);
            break;
        case 2:
            paramSensors->minCriticalValue[pos] = atoi(token);
            break;
        case 3:
            paramSensors->minWarnValue[pos] = atoi(token);
            break;
        case 5:
            paramSensors->maxWarnValue[pos] = atoi(token);
            break;
        case 6:
            paramSensors->maxCriticalValue[pos] = atoi(token);
            break;
        default:
            break;
        }
        column++;
        token = strtok(NULL, ";");
    }
}

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

    printf("test1\n");
    system("sudo modprobe can ; sudo modprobe can_raw ; sudo modprobe vcan");

    printf("test2\n");
#if USE_VCAN
    system("sudo ip link add dev vcan0 type vcan ; sudo ip link set up vcan0");
#else

    printf("test3\n");
    char sys_cmd_can[CAN_CMD_LENGHT];
    snprintf(sys_cmd_can, sizeof(sys_cmd_can), "sudo ip link set %s type can bitrate 100000 ; sudo ip link set %s up", CAN_INTERFACE, CAN_INTERFACE);

    printf("test4\n");
    system(sys_cmd_can);
#endif

	printf("CAN Sockets init\r\n");

	if ((socket_can = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("errCreateCANSocket");
		return errCreateCANSocket;
	}

    int enable_fd = 1;
    if (setsockopt(socket_can, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &enable_fd, sizeof(enable_fd)) < 0) {
        perror("errEnableCANFD");
        return errEnableCANFD;
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

    struct can_filter rfilter[1];
    rfilter[0].can_id = CAN_ID_OBDH;
    rfilter[0].can_mask = CAN_SFF_MASK;
    setsockopt(socket_can, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

	return ret;
}

/**
 * \brief function to initialize the UDP socket
 *
 * \return statusErrDef that values:
 * - errCreateUDPSocket when the UDP socket creation fails
 * - errSetUDPSocketBufSize when the UDP socket buffer size cannot be applied
 * - errGetUDPSocketFlags UDP socket flags cannot be read
 * - errSetUDPSocketNonBlocking when the UDP sucket non blocking flag cannot be set
 * - errBindUDPAddr when the UDP address cannot be bound to the UDP socket
 * - noError when the function exits successfully.
 */
statusErrDef initUDPSocket() {
	statusErrDef ret = noError;
    struct sockaddr_in serverAddr;

    // Create UDP socket
    socket_udp = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_udp < 0) {
        perror("Socket creation failed");
        return errCreateUDPSocket;
    }

	int buf_size = UDP_SOCKET_BUFFER_SIZE;
	if(setsockopt(socket_udp, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size)) == -1){
		perror("errSetUDPSocketBufSize");
		return errSetUDPSocketBufSize;
	}

	int flags = fcntl(socket_udp, F_GETFL, 0);
	if(flags == -1) {
		perror("errGetUDPSocketFlags");
		return errGetUDPSocketFlags;
	}
    if(fcntl(socket_udp, F_SETFL, flags | O_NONBLOCK) == -1) {
		perror("errSetUDPSocketNonBlocking");
		return errSetUDPSocketNonBlocking;
	}

    // Bind to UDP port
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(UDP_TELECOMMAND_PORT);

    if (bind(socket_udp, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Bind failed");
        return errBindUDPAddr;
    }


    printf("Listening for cFS packets on UDP port %d ...\n", UDP_TELECOMMAND_PORT);
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
	ret = initSensorParamCSV();
	if(ret != noError)
		return ret;
	ret = initCANSocket();
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
	ret = initUDPSocket();
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
