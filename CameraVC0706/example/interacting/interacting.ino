#include <Camera.h>
#include <CameraVC0706.h>
#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <SD.h>

void showUsage(SoftwareSerial *out, unsigned char section) {
    switch(section) {
        case 0:
            out->println("'T' > TV off/on.");
            out->println("'o' > Output Resolution.");
            out->println("'t' > Take a Picture.");
            out->println("'M' > Set Motion Monitoring.");
            out->println("'m' > Get Motion Monitoring.");
            out->println("'d' > Detect Monitoring.");
            out->println("'H' > Set Horizontal Mirror Status.");
            out->println("'h' > Get Horizontal Mirror Status.");
            out->println("'r' > Reset.");
            out->println("'I' > Set Image Compression.");
            out->println("'i' > Get Image Compression.");
            out->println("'v' > Version.");
            break;
        case 1:
            out->println("TV:");
            out->println("'0' > On.");
            out->println("'1' > Off.");
            break;
        case 2:
            out->println("Output Output Resolution:");
            out->println("'0' > 160x120.");
            out->println("'1' > 320x240.");
            out->println("'2' > 640x480.");
            break;
        case 3:
            out->println("Set Motion Monitoring:");
            out->println("'0' > On.");
            out->println("'1' > Off.");
            break;
        case 4:
            out->println("Set Horizontal Mirror Status:");
            out->println("'0' > On.");
            out->println("'1' > Off.");
            break;
        case 5:
            out->println("Set Image Compression:");
            out->println("'0..10' > n * 10.");
            break;
    }

}

unsigned char readParam(SoftwareSerial *in) {
    unsigned char op = 0;
    in->print("#> ");
    while(!in->available());
    op = in->read();
    while(in->available() && in->read());
    return op;
}

void captureAndSave(CameraVC0706 *cam) {
    File imageFile;
    imageFile = SD.open("photo.jpg", FILE_WRITE);
    imageFile.seek(0);
	cam->executeBufferControl(CameraVC0706::RESUME_FRAME);
	delay(10);
	if (cam->capture()) {
		unsigned int frameLength = cam->getFrameLength();
        unsigned char b[64];
        unsigned int remaining, done = 0;
        unsigned int readFremeLength = 0;
        while (done < frameLength) {
            remaining = frameLength - done;
            readFremeLength = cam->readFrame(b, done, done, (remaining < 64) ? remaining : 64);
            imageFile.write(b, readFremeLength);
            done += readFremeLength;
        }
	}
    imageFile.close();
}

void setup() {
    SoftwareSerial serial(6, 7);
    CameraVC0706 cam(&Serial, &serial);
    serial.begin(115200);
    cam.begin(115200);
    serial.println("Camera initialized.");
    //if (!SD.begin(4)) {
    //    serial.println("Initialization failed!");
    //    return;
    //}
    serial.println("Initialization done.");
    unsigned char c, arg0, arg1;
    while (true) {
        showUsage(&serial, 0);
        unsigned char arg0 = readParam(&serial);

        switch (arg0) {

            case 'T':
                showUsage(&serial, 1);
                arg1 = readParam(&serial);
                c = (arg1 == '1') ? 1 : 0;
                serial.println(cam.setTVOutput(c) ? "OK" : "Error");
                break;

            case 'o':
                showUsage(&serial, 2);
                arg1 = readParam(&serial);
                switch(arg1) {
                    case '0':
                        c = CameraVC0706::RES_160X120;
                        break;
                    case '1':
                        c = CameraVC0706::RES_320X240;
                        break;
                    case '2':
                        c = CameraVC0706::RES_640X480;
                        break;
                    default:
                        c = CameraVC0706::RES_160X120;
                }
                serial.println(cam.setOutputResolution(c) ? "OK" : "Error");
                break;

            case 't':
                captureAndSave(&cam);
                serial.println("Soon!");
                break;

            case 'M':
                showUsage(&serial, 3);
                arg1 = readParam(&serial);
                c = (arg1 == '1') ? 1 : 0;
                serial.println(cam.setMotionMonitoring(c) ? "OK" : "Error");
                break;

            case 'm':
                serial.println(cam.getMotionMonitoringStatus());
                break;

            case 'd':
                serial.println("Soon!");
                break;

            case 'H':
                showUsage(&serial, 4);
                arg1 = readParam(&serial);
                c = (arg1 == '1') ? 1 : 0;
                serial.println(cam.setHorizontalMirror(CameraVC0706::UART, c) ? "OK" : "Error");
                break;

            case 'h':
                serial.println(cam.getHorizontalMirrorStatus());
                break;

            case 'r':
                serial.println(cam.reset() ? "OK" : "Error");
                break;

            case 'I':
                showUsage(&serial, 5);
                arg1 = readParam(&serial);
                c = arg1 - '0';
                serial.println(cam.setCompression(c*10) ? "OK" : "Error");
                break;

            case 'i':
                serial.println(cam.getCompression());
                break;

            case 'v':
                serial.println(cam.getVersion());
                break;

            default:
                serial.println("Not understood.");
        }
    }
}

void loop() {
}
