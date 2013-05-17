#include <CameraVC0706.cpp>

void captureAndSave(CameraVC0706 *cam, char *fileName);
void showUsage();
int getBoudFromChar(char c);
void captureCallback(void *param);

int main(int argc, char *argv[]) {

	if (argc < 3) {
		showUsage();
		exit(0);
	}

	int baud = getBoudFromChar(argv[1][0]);
	printf("Unsing baud %x\n", baud);

	CameraVC0706 cam((char*) "/dev/ttyUSB0");
	if (!cam.begin(baud)) {
		printf("Cannot begin.\n");
		exit(1);
	}
	
	switch (argv[2][0]) {

		unsigned char b, c, d;
		case 'B':
			if (argc < 4) {
				printf("Baud rate not specified.\n");
				exit(1);
			}
			baud = getBoudFromChar(argv[3][0]);
			printf("Baud rate: %d\n", baud);
			cam.setBoudRate(baud);
			break;

		case 'T':
			printf("TV: ");
			if (argv[3][0] == '1') {
				printf("on\n");
				c = 1;
			} else {
				printf("off\n");
				c = 0;
			}
			cam.setTVOutput(c);
			break;

		case 'o':
			printf("Output resolution: ");
			switch(argv[3][0]) {

				case '0':
					printf("160x120\n");
					c = CameraVC0706::OR_160x120;
					break;

				case '1':
					printf("320x240\n");
					c = CameraVC0706::OR_320x240;
					break;

				case '2':
					printf("640x480\n");
					c = CameraVC0706::OR_640x480;
					break;
				default:
					printf("Error, invalid resolution.\n");
					exit(1);
			}
			cam.setOutputResolution(c);
			break;

		case 't':
			if (argc < 4) {
				printf("File name not specified.\n");
				exit(1);
			}
			printf("Capturing to file: %s.\n", argv[3]);
			captureAndSave(&cam, argv[3]);
			break;
			
		case 'M':
			if (argc < 4) {
				printf("Detection mode not specified.\n");
				exit(1);
			}
			c = atoi(argv[3]);
			printf("Setting motion monitoring to: %d.\n", c);
			cam.setMotionMonitoring(c & 0x01);
			break;
			
		case 'm':
			c = cam.getMotionMonitoringStatus();
			printf("Getting motion monitoring: %d.\n", c);
			break;
			
		case 'd':
			printf("Detect motion.");
			cam.pollMotionMonitoring(120, captureCallback);
			break;
            
		case 'C':
			if (argc < 5) {
				printf("Cannot set color control, insufficient param.\n");
				exit(1);
			}
			b = (unsigned char) atoi(argv[3]);
			c = (unsigned char) atoi(argv[4]);
			if (cam.setColorControl(b, c)) {
                printf("Set color control by %s with flag: %d.\n", (b ? "UART" : "GPIO"), c);
			} else {
                printf("Error on setting color control by %s with flag: %d.\n", (b ? "UART" : "GPIO"), c);
            }
			break;
			
		case 'c':
			printf("Get color control. ");
			c = cam.getColorControlStatus();
            printf("By: %s and flag: %d.\n", (c & 0x01 ? "UART" : "GPIO"), (c >> 1) & 0x03);
			break;
            
		case 'H':
			if (argc < 5) {
				printf("Cannot set horizontal mirror, insufficient param.\n");
				exit(1);
			}
			b = (unsigned char) atoi(argv[3]);
			c = (unsigned char) atoi(argv[4]);
			if (cam.setHorizontalMirror(b, c)) {
                printf("Set horizontal mirror by %s with flag: %d.\n", (b ? "GPIO" : "UART"), c);
			} else {
                printf("Error on setting horizontal mirror by %s with flag: %d.\n", (b ? "GPIO" : "UART"), c);
            }
			break;
			
		case 'h':
			printf("Get horizontal mirror. ");
			c = cam.getHorizontalMirrorStatus();
            printf("By: %s and flag: %d.\n", (c & 0x01 ? "GPIO" : "UART"), (c >> 1) & 0x01);
			break;
			
		case 'r':
			printf("Reseting. ");
			if (cam.reset()) {
				printf("OK\n");
			} else {
				printf("Error\n");
			}	
            break;
			
		case 's':
			if (argc < 7) {
				printf("Cannot set OSD characters, insufficient param.\n");
				exit(1);
			}
			b = (unsigned char) atoi(argv[3]);
			c = (unsigned char) atoi(argv[4]);
			d = (unsigned char) atoi(argv[5]);
			printf("Adding OSD Characters.");
			printf(" x: %d, y: %d, len: %d, string: %s.\n", b, c, d, argv[6]);
			if (cam.setOsdCharacters(b, c, (unsigned char*)argv[6], d)) {
				printf("OK\n");
			} else {
				printf("Error\n");
			}	
            break;
			
		case 'i':
			printf("Get image compression. ");
			b = cam.getCompression();
			printf(": %d\n",  b);
            break;
			
		case 'I':
			if (argc < 4) {
				printf("Cannot set image compression, insufficient param.\n");
				exit(1);
			}
			b = (unsigned char) atoi(argv[3]);
			printf("Set image compression to %d.", b);
			if (cam.setCompression(b)) {
				printf("OK\n");
			} else {
				printf("Error\n");
			}	
            break;
			
		default:
			printf("%f\n", cam.getVersion());
	}

	cam.close();
	return 0;
}

void captureCallback(void *param) {
	captureAndSave((CameraVC0706 *) param, (char*)"photo1.jpg");
}

void captureAndSave(CameraVC0706 *cam, char *fileName) {
	cam->executeBufferControl(CameraVC0706::RESUME_FRAME);
	usleep(100000);
	if (cam->capture()) {
		FILE* destinatioFile;
		destinatioFile = fopen(fileName, "w");
		if (destinatioFile == NULL) {
			printf("@captureAndSave: cannot open file.\n");
		}
		int frameLength = cam->getFrameLength();
		printf("@captureAndSave: frame length: %d\n", frameLength);
		if (frameLength > 0) {
			unsigned char buffer[frameLength];
			int readFremeLength = cam->readFrame(&buffer[0], 0, 0,
					(int) frameLength);
			printf("@captureAndSave: readFremeLength: %d\n\n", readFremeLength);
			if (readFremeLength == frameLength) {
				fwrite(&buffer[0], sizeof(unsigned char), frameLength,
						destinatioFile);
			} else {
				printf("@captureAndSave: read %d  differs from %d.\n", readFremeLength,
						frameLength);
			}
		} else {
			printf("@captureAndSave: frame length error, returned 0.\n");
		}
	} else {
		printf("@captureAndSave: cannot campure.\n");
	}
}

void showUsage() {
    printf("# prog 5 B 0/1/2/3/4/5      @115200 Baud Rate 9600/19200/38400/57600/115200.\n");
    printf("# prog 5 T 0/1              @115200 TV off/on.\n");
    printf("# prog 5 o 0/1/2            @115200 Resolution 160x120/320x240/640x480.\n");
    printf("# prog 5 t 'filename.jpg'   @115200 Take a Picture.\n");
    printf("# prog 5 M 0/1              @115200 Set Motion Monitoring.\n");
    printf("# prog 5 m                  @115200 Get Motion Monitoring.\n");
    printf("# prog 5 d                  @115200 Detect Monitoring.\n");
    printf("# prog 5 C 0/1 0/1/2        @115200 Set Color Control GPIO/UART AUTO/MANUAL/MANUAL_BLACK_WHITE.\n");
    printf("# prog 5 c                  @115200 Get Color Control.\n");
    printf("# prog 5 H 0/1 0/1          @115200 Set Horizontal Mirror Status GPIO/UART DoNotShow/Show.\n");
    printf("# prog 5 h                  @115200 Get Horizontal Mirror Status.\n");
    printf("# prog 5 s x y len str      @115200 Add OSD Characters.\n");
    printf("# prog 5 I comp             @115200 Set Image Compression.\n");
    printf("# prog 5 i                  @115200 Get Image Compression.\n");
    printf("# prog 5 r                  @115200 Reset.\n");
    printf("# prog 5 v                  @115200 Version.\n");
}

int getBoudFromChar(char c) {
	int baud = CameraVC0706::B_115200;
	switch(c) {
		case '0':
			baud = CameraVC0706::B_9600;
			break;
		case '2':
			baud = CameraVC0706::B_19200;
			break;
		case '3':
			baud = CameraVC0706::B_38400;
			break;
		case '4':
			baud = CameraVC0706::B_57600;
			break;
		case '5':
			baud = CameraVC0706::B_115200;
			break;
	}
	return baud;
}
