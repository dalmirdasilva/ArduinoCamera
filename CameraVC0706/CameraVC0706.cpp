#include "CameraVC0706.h"

CameraVC0706::CameraVC0706(SoftwareSerial *serial) : serial(serial) {
	rxBufferPointer = 0;
	serialNumber = 0x00;
	framePointer = 0;
}

bool CameraVC0706::begin(unsigned int baud) {
	serial->begin(baud);
	return true;
}

bool CameraVC0706::close() {
	serial->end();
	return true;
}

bool CameraVC0706::capture() {
	return executeBufferControl(STOP_CURRENT_FRAME);
}

bool CameraVC0706::resume() {
	return executeBufferControl(RESUME_FRAME);
}

bool CameraVC0706::executeBufferControl(unsigned char control) {
	unsigned char args[] = {(unsigned char) (control & 0x03)};
	return executeCommand(FBUF_CTRL, args, sizeof(args), 5);
}

unsigned int CameraVC0706::readFrame(unsigned char *buf, unsigned int frameOffset,
		unsigned int bufferOffset, unsigned int len) {
	unsigned int bytesRead = 0;
	unsigned char args[] = {0x00, 0x0a, 
			0x00, 0x00, ((frameOffset >> 8) & 0xff), frameOffset & 0xff, 
			0x00, 0x00, ((len >> 8) & 0xff), len & 0xff, 
			(VC0760_CAMERA_DELAY >> 8) & 0xff, VC0760_CAMERA_DELAY & 0xff};

	if (!executeCommand(READ_FBUF, args, sizeof(args), 5)) {
		return 0;
	}
	while (bytesRead < len) {
		delay(10);
		bytesRead += read(&buf[bufferOffset + bytesRead], len - bytesRead);
	}
	readResponse(5);
	return bytesRead;
}

unsigned int CameraVC0706::getFrameLength() {
	unsigned int frameLength = 0;
	unsigned char args[] = {0x00};
	if (!executeCommand(GET_FBUF_LEN, args, sizeof(args), 9)
			&& rxBuffer[4] == 0x04) {
		return 0;
	}
	frameLength |= rxBuffer[7];
	frameLength <<= 8;
	frameLength |= rxBuffer[8];
	return frameLength;
}

bool CameraVC0706::setHorizontalMirror(unsigned char by,
		unsigned char mirrorMode) {
	unsigned char args[] = {(unsigned char) (by & 0x01),
			(unsigned char) (mirrorMode & 0x01)};
	return executeCommand(MIRROR_CTRL, args, sizeof(args), 5);
}

unsigned char CameraVC0706::getHorizontalMirrorStatus() {
	unsigned char args[] = {};
	bool run = executeCommand(MIRROR_STATUS, args, sizeof(args), 7);
	unsigned char status = 0;
	if (run) {
		status = (rxBuffer[6] & 0x01) | ((rxBuffer[5] << 1) & 0x02);
	}
	return status;
}

bool CameraVC0706::setColorControl(unsigned char by,
		unsigned char colorControlMode) {
	unsigned char args[] = {(unsigned char) (by & 0x01),
			(unsigned char) (colorControlMode & 0x01)};
	return executeCommand(COLOR_CTRL, args, sizeof(args), 5);
}

unsigned char CameraVC0706::getColorControlStatus() {
	unsigned char args[] = {};
	bool run = executeCommand(COLOR_STATUS, args, sizeof(args), 8);
	unsigned char status = 0;
	if (run) {
		status = (rxBuffer[5] & 0x01) | ((rxBuffer[6] << 2) & 0x06);
	}
	return status;
}

bool CameraVC0706::setOutputResolution(unsigned char resolution) {
	unsigned char args[] = {0x04, 0x01, 0x00, 0x19, resolution};
	return executeCommand(WRITE_DATA, args, sizeof(args), 5);
}

bool CameraVC0706::setMotionMonitoring(bool monitor) {
	unsigned char args[] = {(unsigned char) (monitor & 0x01)};
	return executeCommand(COMM_MOTION_CTRL, args, sizeof(args), 5);
}

bool CameraVC0706::getMotionMonitoringStatus() {
	unsigned char args[] = {};
	return (executeCommand(COMM_MOTION_STATUS, args, sizeof(args), 6))
			&& rxBuffer[5];
}

bool CameraVC0706::pollMotionMonitoring(unsigned int timeout, void (*callback)(void *)) {
	long start = 0, now = 0;
	bool detected = 0;
	start = millis();
	do {
		readResponse(4);
		detected = verifyResponse(COMM_MOTION_DETECTED);
		if (detected && callback != 0) {
			callback(this);
		} else {
			now = millis();
		}
	} while (((now - start) < timeout) && !detected);
	return detected;
}

unsigned int CameraVC0706::write(unsigned char *buf, unsigned int size) {
	unsigned int txLength = 0;

#if VC0760_DEBUG == 1
	Serial.print("About to write: ");
	Serial.print(size);
	Serial.println(" bytes.");
#endif

	txLength = serial->write(&buf[0], size);

#if VC0760_DEBUG == 1
	if (txLength < 0) {
		Serial.println("UART TX error.");
	} else if (txLength != size) {
		Serial.print("Sent bytes ");
		Serial.print(txLength);
		Serial.print(" differs from the size to be send ");
		Serial.println(size);
	}
#endif

	return txLength;
}

unsigned int CameraVC0706::read(unsigned char *buf, unsigned int size) {
	unsigned char c = 0;
	rxBufferPointer = 0;
	while (size-- > 0 && (c = serial->read()) != -1) {
		rxBuffer[rxBufferPointer++] = c;
	}
	
#if VC0760_DEBUG == 1
	if (c < 0) {
		Serial.println("Error on read.");
	} else if (rxBufferPointer == 0) {
		Serial.println("No data received on read.");
	} else if (rxBufferPointer != size) {
		Serial.print("Read bytes: ");
		Serial.print(rxBufferPointer);
		Serial.print(" differs from the size to be read: ");
		Serial.println(size);
	} else {
		Serial.print("It matches! ");
		Serial.print(rxBufferPointer);
		Serial.print(" bytes read when expecting: ");
		Serial.println(size);
	}
#endif

	return rxBufferPointer;
}

bool CameraVC0706::executeCommand(unsigned char cmd, unsigned char *args,
		unsigned int argc, unsigned int responseLength) {
	if (!sendCommand(cmd, args, argc)) {
		return false;
	}
	delay(50);
	if (!readResponse(responseLength)) {
		return false;
	}
	if (!verifyResponse(cmd)) {
		return false;
	}
	return true;
}

unsigned int CameraVC0706::sendCommand(unsigned char cmd, unsigned char *args,
		unsigned int argc) {
	unsigned int length;
	unsigned char buf[4 + argc];
	buf[0] = VC0760_PROTOCOL_SIGN_TX;
	buf[1] = serialNumber;
	buf[2] = cmd;
	buf[3] = (unsigned char) (argc & 0xff);
	memcpy(&buf[4], args, argc);
	printBuff(buf, sizeof(buf));
	length = write(buf, sizeof(buf));

#if VC0760_DEBUG == 1
	Serial.print(length);
	Serial.println(" bytes written.");
#endif

	if (length != sizeof(buf)) {

#if VC0760_DEBUG == 1
		Serial.print("Cannot write. Returned: ");
		Serial.println(length);
#endif

		return 0;
	}
	return length;
}

bool CameraVC0706::verifyResponse(unsigned char cmd) {
	if ((rxBuffer[0] != VC0760_PROTOCOL_SIGN_RX)
			|| (rxBuffer[1] != serialNumber) || (rxBuffer[2] != cmd)
			|| (rxBuffer[3] != 0x00)) {
		return false;
	}
	return true;
}

unsigned int CameraVC0706::readResponse(unsigned int length) {
	rxBufferPointer = read(rxBuffer, length);
	printBuff(rxBuffer, rxBufferPointer);
	return rxBufferPointer;
}

void CameraVC0706::printBuff(unsigned char *buf, unsigned int c) {

#if VC0760_DEBUG == 1
	Serial.println("Printing buffer:");
	for (unsigned int i = 0; i < c; i++) {
		Serial.print(i);
		Serial.print(" : ");
		Serial.println(buf[i]);
	}
#endif

}

bool CameraVC0706::reset() {
	unsigned char args[] = {};
	bool run = executeCommand(SYSTEM_RESET, args, sizeof(args), 5);

#if VC0760_DEBUG == 1
	if (run) {
		Serial.println("Waiting the system to reset.");
		delay(10);
	}
#endif

	return run;
}

float CameraVC0706::getVersion() {
	unsigned int i = 0;
	unsigned char args[] = {};
	if (!executeCommand(GEN_VERSION, args, sizeof(args), 18)) {
		return 0.0;
	}
	float version = 0.0;
	while (rxBuffer[i++] != ' ')
		;
	version += rxBuffer[i] - '0';
	version += 0.1 * (rxBuffer[i + 2] - '0');
	version += 0.01 * (rxBuffer[i + 3] - '0');
	return version;
}

bool CameraVC0706::setOsdCharacters(unsigned char x, unsigned char y,
		unsigned char *str, unsigned char len) {
	if (len > 14) {
		len = 14;
	}
	unsigned char args[2 + len];
	args[0] = len;
	args[1] = ((x << 6) & 0x60) | (y & 0x1f);
	memcpy(&args[2], str, len);
	return executeCommand(OSD_ADD_CHAR, args, sizeof(args), 5);
}

bool CameraVC0706::setCompression(unsigned char compression) {
	unsigned char args[] = {0x01, 0x01, 0x12, 0x04, compression};
	return executeCommand(WRITE_DATA, args, sizeof(args), 5);
}

unsigned char CameraVC0706::getCompression() {
	unsigned char args[] = {0x01, 0x01, 0x12, 0x04};
	bool run = executeCommand(READ_DATA, args, sizeof(args), 6);
	unsigned char compression = 0;
	if (run) {
		compression = rxBuffer[5];
	}
	return compression;
}

bool CameraVC0706::setTVOutput(unsigned char onOff) {
	unsigned char args[] = {onOff & 0x01};
	return executeCommand(TV_OUT_CTRL, args, sizeof(args), 5);
}

bool CameraVC0706::setBoudRate(unsigned int baudRate) {
	this->baudRate = baudRate;
	unsigned char args[] = {0x01, (baudRate >> 8) & 0xff, baudRate & 0xff};
	return executeCommand(SET_PORT, args, sizeof(args), 5);
}
