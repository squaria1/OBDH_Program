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
statusErrDef initSensorValArrays();
statusErrDef writeSensorsValFile(const char* fileName, int index);
statusErrDef initCANSocket();
statusErrDef initUDPSocket();

/**
 * \struct paramSensors
 * \brief array of struct containing the parameters of
 * each sensors on the spacecraft.
 * Array size determined by the number of sensors
 * defined in "paramSensors.csv".
 *
 */
struct paramSensorsStruct* paramSensors;

/**
 * \struct sensorsVal
 * \brief array of struct containing the history of
 * sensor values of every sensors.
 * Array size determined by the number of sensors
 * defined in "paramSensors.csv".
 *
 */
struct sensorsValStruct* sensorsVal;

/**
 * \struct beginTimeOBDH
 * \brief struct of the OBDH program begin time.
 *
 */
struct timespec beginTimeOBDH;

/**
 * \struct endTimeOBDH
 * \brief struct of the OBDH program end time.
 *
 */
struct timespec endTimeOBDH;

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
    char filePath[MAX_PATH_LENGHT];

    sprintf(filePath, "%s%s",OUTPUT_FILES_DIR, PARAM_SENSORS_CSV_FILENAME);
	lineCountSensorParamCSV = countFileLines(filePath);
    if(lineCountSensorParamCSV == -1) {
		return errOpenParamSensorsFile;
    }
    if (lineCountSensorParamCSV <= 1) {
        printf("No sensors found in file.\n");
        paramSensors = NULL; // Handle empty file case
        return noError; // Or return an error if this is invalid
    }

    lineCountSensorParamCSV--; // Adjust for header

    // Allocate the struct itself
    paramSensors = (struct paramSensorsStruct*)malloc(sizeof(struct paramSensorsStruct));
    if (paramSensors == NULL) {
        perror("Error allocating memory for struct");
        return errAllocParamSensorStruct;
    }

    paramSensors->id = (uint16_t*)malloc(lineCountSensorParamCSV * sizeof(uint16_t));
    paramSensors->minCriticalValue = (int32_t*)malloc(lineCountSensorParamCSV * sizeof(int32_t));
    paramSensors->minWarnValue = (int32_t*)malloc(lineCountSensorParamCSV * sizeof(int32_t));
    paramSensors->currentValue = (int32_t*)malloc(lineCountSensorParamCSV * sizeof(int32_t));
    paramSensors->maxWarnValue = (int32_t*)malloc(lineCountSensorParamCSV * sizeof(int32_t));
    paramSensors->maxCriticalValue = (int32_t*)malloc(lineCountSensorParamCSV * sizeof(int32_t));

    // Check all allocations
    if (paramSensors->id == NULL || paramSensors->minCriticalValue == NULL ||
        paramSensors->minWarnValue == NULL || paramSensors->currentValue == NULL ||
        paramSensors->maxWarnValue == NULL || paramSensors->maxCriticalValue == NULL) {
        perror("errAllocParamSensorStruct");
        // Free any successful allocations to avoid leaks
        free(paramSensors->id);
        free(paramSensors->minCriticalValue);
        free(paramSensors->minWarnValue);
        free(paramSensors->currentValue);
        free(paramSensors->maxWarnValue);
        free(paramSensors->maxCriticalValue);
        free(paramSensors);
        paramSensors = NULL;
        return errAllocParamSensorStruct;
    }

    // Initialize all arrays to 0
    memset(paramSensors->id, 0, lineCountSensorParamCSV * sizeof(uint16_t));
    memset(paramSensors->minCriticalValue, 0, lineCountSensorParamCSV * sizeof(int32_t));
    memset(paramSensors->minWarnValue, 0, lineCountSensorParamCSV * sizeof(int32_t));
    memset(paramSensors->currentValue, 0, lineCountSensorParamCSV * sizeof(int32_t));
    memset(paramSensors->maxWarnValue, 0, lineCountSensorParamCSV * sizeof(int32_t));
    memset(paramSensors->maxCriticalValue, 0, lineCountSensorParamCSV * sizeof(int32_t));

	ret = readParamSensorsFile(filePath);

    if (paramSensors != NULL) {
        printf("Number of sensors: %d\n", lineCountSensorParamCSV);
        for (int i = 0; i < lineCountSensorParamCSV; i++) {
            printf("Sensor %d: id=0x%04X, minCrit=%d, minWarn=%d, maxWarn=%d, maxCrit=%d\n",
                   i, paramSensors->id[i], paramSensors->minCriticalValue[i],
                   paramSensors->minWarnValue[i], paramSensors->maxWarnValue[i],
                   paramSensors->maxCriticalValue[i]);
        }
    } else {
        printf("paramSensors is NULL\n");
    }

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
    printf("filename: %s \n", filename);
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("File open error");
        return -1;
    }

    int count = 0;
    int ch;

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
    // Skip the header line
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        // File is empty or unreadable
        return errOpenParamSensorsFile;
    }

    while (fgets(line, sizeof(line), file) != NULL && pos <= lineCountSensorParamCSV)
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
        switch (column) {
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
 * \brief function to initialize the sensorVal structure
 * and the sensor values files.
 *
 * \return statusErrDef that values:
 * - errAllocSensorValStruct when the sensorVal structure memory allocation fails
 * - errOpenParamSensorsFile when the paramSensors.csv file fails to open
 * - noError when the function exits successfully.
 */
statusErrDef initSensorValArrays() {
	statusErrDef ret = noError;
    char filePath[MAX_PATH_LENGHT];

    // Allocate array of structs for lineCountSensorParamCSV sensors
    sensorsVal = (struct sensorsValStruct*)malloc(lineCountSensorParamCSV * sizeof(struct sensorsValStruct));
    if (sensorsVal == NULL) {
        perror("errAllocSensorValStruct");
        return errAllocSensorsValStruct;
    }

    // Initialize all elements to zero
    memset(sensorsVal, 0, lineCountSensorParamCSV * sizeof(struct sensorsValStruct));

    for(int i = 0; i < lineCountSensorParamCSV; i++) {
        sprintf(filePath, "%s0x%04X.csv",OUTPUT_FILES_DIR, paramSensors->id[i]);
        sensorsVal[i].id = paramSensors->id[i];
        ret = writeSensorsValFile(filePath, i);
    }

    return ret;
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
statusErrDef writeSensorsValFile(const char* fileName, int index) {
    printf("filename: %s\n", fileName);
    sensorsVal[index].sensorFile = fopen(fileName, "w");
    if (sensorsVal[index].sensorFile == NULL) {
        perror("File open error");
        return errOpenSensorsValFile;
    }

    // Write CSV header
    fprintf(sensorsVal[index].sensorFile, "Timestamp since program start (sec);Value\n");

    // Don’t close here if you want to write more later
    return noError;
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

#if USE_VCAN
    system("sudo modprobe can ; sudo modprobe can_raw ; sudo modprobe vcan ; sudo ip link add dev vcan0 type vcan ; sudo ip link set vcan0 up");
#else
    char sys_cmd_can[CAN_CMD_LENGHT];
    snprintf(sys_cmd_can, sizeof(sys_cmd_can), "sudo ip link set %s down ; sudo ip link set %s type can bitrate 100000 ; sudo ip link set %s up", CAN_INTERFACE, CAN_INTERFACE, CAN_INTERFACE);

    system(sys_cmd_can);
#endif

	printf("CAN Sockets init\r\n");

	if ((socket_can = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("errCreateCANSocket");
		return errCreateCANSocket;
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

    ret = initSensorValArrays();
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
	clock_gettime(CLOCK_MONOTONIC, &beginTimeOBDH);
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
