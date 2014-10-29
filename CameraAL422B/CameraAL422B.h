/**
 * Arduino - CameraAL422B implementation.
 * 
 * CameraAL422B.h
 * 
 * The class CameraAL422B.
 * 
 * @author Dalmir da Silva <dalmirdasilva@gmail.com>
 */

#ifndef __ARDUINO_DRIVER_CAMERA_AL422B_H__
#define __ARDUINO_DRIVER_CAMERA_AL422B_H__ 1

// Fast IO
#define digitalWriteFast(pin, state)    ((tate) == LOW) ? digitalWriteLowFast((pin)) : digitalWriteHighFast((pin))
#define digitalWriteLowFast(pin)        ((pin) < 8) ? PORTD &= ~(1 << (pin)) : ((pin) > 13) ? PORTC &= ~(1 << ((pin) - 14)) : PORTB &= ~(1 << ((pin) - 8))
#define digitalWriteHighFast(pin)       ((pin) < 8) ? PORTD |= (1 << (pin)) : ((pin) > 13) ? PORTC |= (1 << ((pin) - 14)) : PORTB |= (1 << ((pin) - 8))
#define digitalReadFast(pin)            ((pin) < 8) ? (PORTD & (1 << (pin))) : ((pin) > 13) ? (PORTC & (1 << ((pin) - 14))) : (PORTB & (1 << ((pin) - 8)))

#include <OutputStream.h>
#include <Camera.h>
#include <Arduino.h>
#include <Wire.h>

/*
1121 static unsigned char ov7670_sm_to_abs(unsigned char v)
1122 {
1123         if ((v & 0x80) == 0)
1124                 return v + 128;
1125         return 128 - (v & 0x7f);
1126 }
1127
1128
1129 static unsigned char ov7670_abs_to_sm(unsigned char v)
1130 {
1131         if (v > 127)
1132                 return v & 0x7f;
1133         return (128 - v) | 0x80;
1134 }*/

class CameraAL422B : public Camera {
private:

    unsigned char (*read)();

    unsigned char vsyncPin;

    unsigned char writeEnPin;

    unsigned char readClockPin;

    unsigned char readResetPin;

    unsigned char address;

    /**
     * Width in pixels.
     */
    int width;

    /**
     * Height in pixels.
     */
    int height;

public:

    union MVFPbits {

        struct {
            unsigned char :2;
            unsigned char BLACK_SUN_EN :1;
            unsigned char :1;
            unsigned char VFLIP :1;
            unsigned char MIRROR :1;
            unsigned char :2;
        };
        unsigned char value;
    };

    enum Mask {

        // Flashlight Mode Select
        STR_OPT_MODE = 0x03,

        // Strobe Request
        STR_OPT_REQUEST = 0x80,

        // Color Gain Control Enable
        STR_OPT_GAIN = 0x40,

        // Horizontal mirror
        MVFP_MIRROR = 0x20,

        // Vertical flip
        MVFP_FLIP = 0x10,

        // Reset
        COM7_RESET = 0x80,

        // Output resolution.
        COM7_RESOLUTION = 0x38,

        // Output format.
        COM7_FORMAT = 0x05,

        // Color bar
        COM7_COLOR_BAR = 0x02,

        // Soft sleep mode
        COM2_SSLEEP = 0x10,

        // Separator
        _ = 0x00,

        // CCIR656 enable
        COM1_CCIR656 = 0x40,

        // Byte swap
        COM3_SWAP = 0x40,

        // Enable scaling
        COM3_SCALEEN = 0x08,

        // Enable downsamp/crop/window
        COM3_DCWEN = 0x04,

        // Use external clock directly
        CLKRC_EXT = 0x40,

        // Mask for internal clock scale
        CLKRC_SCALE = 0x3f,

        // Enable fast AGC/AEC
        COM8_FASTAEC = 0x80,

        // Unlimited AEC step size
        COM8_AECSTEP = 0x40,

        // Band filter enable
        COM8_BFILT = 0x20,

        // Auto gain enable
        COM8_AGC = 0x04,

        // White balance enable
        COM8_AWB = 0x02,

        // Auto exposure enable
        COM8_AEC = 0x01,

        // HSYNC instead of HREF
        COM10_HSYNC = 0x40,

        // Suppress PCLK on horiz blank
        COM10_PCLK_HB = 0x20,

        // Reverse HREF
        COM10_HREF_REV = 0x08,

        // VSYNC on clock leading edge
        COM10_VS_LEAD = 0x04,

        // VSYNC negative
        COM10_VS_NEG = 0x02,

        // HSYNC negative
        COM10_HS_NEG = 0x01,

        // UYVY or VYUY - see com13
        TSLB_YLAST = 0x04,

        // Night mode enable
        COM11_NIGHT = 0x80,

        // Two bit NM frame rate
        COM11_NMFR = 0x60,

        // Auto detect 50/60 Hz
        COM11_HZAUTO = 0x10,

        // Manual 50Hz select
        COM11_50HZ = 0x08,

        // Exp
        COM11_EXP = 0x02,

        // HREF always
        COM12_HREF = 0x80,

        // Gamma enable
        COM13_GAMMA = 0x80,

        // UV saturation auto adjustment
        COM13_UVSAT = 0x40,

        // V before U - w/TSLB
        COM13_UVSWAP = 0x01,

        // DCW/PCLK-scale enable
        COM14_DCWEN = 0x10,

        // Data range 10 to F0
        COM15_R10F0 = 0x00,

        // Data range 01 to FE
        COM15_R01FE = 0x80,

        // Data range 00 to FF
        COM15_R00FF = 0xc0,

        // RGB options
        COM15_RGB = 0x30,

        // AWB gain enable
        COM16_AWBGAIN = 0x08,

        // AEC window - must match COM4
        COM17_AECWIN = 0xc0,

        // DSP Color bar
        COM17_CBAR = 0x08,

        // White pixel correction enable
        R76_WHTPCOR = 0x40,

        // Black pixel correction enable
        REG76_BLKPCOR = 0x20,

        // Edge enhancement higher limit
        REG76_EDGE = 0x1f
    };

    enum Register {

        // Gain lower 8 bits (rest in vref)
        GAIN = 0x00,

        // blue gain
        BLUE = 0x01,

        // red gain
        RED = 0x02,

        // Pieces of GAIN, VSTART, VSTOP
        VREF = 0x03,

        // Control 1
        COM1 = 0x04,

        // U/B Average level
        BAVE = 0x05,

        // Y/Gb Average level
        GBAVE = 0x06,

        // AEC MS 5 bits
        AECHH = 0x07,

        // V/R Average level
        RAVE = 0x08,

        // Control 2
        COM2 = 0x09,

        // Product ID MSB
        PID = 0x0a,

        // Product ID LSB
        VER = 0x0b,

        // Control 3
        COM3 = 0x0c,

        // Control 4
        COM4 = 0x0d,

        // All "reserved"
        COM5 = 0x0e,

        // Control 6
        COM6 = 0x0f,

        // More bits of AEC value
        AECH = 0x10,

        // Clocl control
        CLKRC = 0x11,

        // Control 7
        COM7 = 0x12,

        // Control 8
        COM8 = 0x13,

        // Control 9  - gain ceiling
        COM9 = 0x14,

        // Control 10
        COM10 = 0x15,

        // Horiz start high bits
        HSTART = 0x17,

        // Horiz stop high bits
        HSTOP = 0x18,

        // Vert start high bits
        VSTART = 0x19,

        // Vert stop high bits
        VSTOP = 0x1a,

        // Pixel delay after HREF
        PSHFT = 0x1b,

        // Manuf. ID high
        MIDH = 0x1c,

        // Manuf. ID low
        MIDL = 0x1d,

        // Mirror / vflip
        MVFP = 0x1e,

        // AGC upper limit.
        AEW = 0x24,

        // AGC lower limit.
        AEB = 0x25,

        // AGC/AEC fast mode op region.
        VPT = 0x26,

        // B Channel Signal Output Bias (effective only when COM6[3] = 1).
        BBIAS = 0x27,

        // Gb Channel Signal Output Bias (effective only when COM6[3] = 1).
        GBBIAS = 0x28,

        // Dummy Pixel Insert MSB.
        EXHCH = 0x2a,

        // Dummy Pixel Insert LSB.
        EXHCL = 0x2b,

        // R Channel Signal Output Bias (effective only when COM6[3] = 1).
        RBIAS = 0x2c,

        // LSB of insert dummy rows in vertical direction (1 bit equals 1 row).
        ADVFL = 0x2d,

        // MSB of insert dummy rows in vertical direction.
        ADVFH = 0x2e,

        // Y/G Channel Average Value.
        YAVE = 0x2f,

        // HSYNC rising edge delay
        HSYST = 0x30,

        // HSYNC falling edge delay
        HSYEN = 0x31,

        // HREF pieces
        HREF = 0x32,

        // Array Current Control.
        CHLF = 0x33,

        // Array Reference Control.
        ARBLM = 0x34,

        // ADC Control.
        ADC_CONTROL = 0x37,

        // ADC and Analog Common Mode Control.
        ACOM = 0x38,

        // ADC Offset Control.
        OFON = 0x39,

        // Line Buffer Test Option.
        TSLB = 0x3a,

        // Control 11
        COM11 = 0x3b,

        // Control 12
        COM12 = 0x3c,

        // Control 13
        COM13 = 0x3d,

        // Control 14
        COM14 = 0x3e,

        // Edge enhancement factor
        EDGE = 0x3f,

        // Control 15
        COM15 = 0x40,

        // Control 16
        COM16 = 0x41,

        // Control 17
        COM17 = 0x42,

        // WB Control 1.
        AWBC1 = 0x43,

        // WB Control 2.
        AWBC2 = 0x44,

        // WB Control 3.
        AWBC3 = 0x45,

        // WB Control 4.
        AWBC4 = 0x46,

        // WB Control 5.
        AWBC5 = 0x47,

        // WB Control 6.
        AWBC6 = 0x48,

        // UV average.
        REG4B = 0x4b,

        // De-noise Strength.
        DNSTH = 0x4c,

        // Dummy row position.
        DM_POS = 0x4d,

        // Matrix Coefficient 1.
        MTX1 = 0x4f,

        // Matrix Coefficient 2.
        MTX2 = 0x50,

        // Matrix Coefficient 3.
        MTX3 = 0x51,

        // Matrix Coefficient 4.
        MTX4 = 0x52,

        // Matrix Coefficient 5.
        MTX5 = 0x53,

        // Matrix Coefficient 6.
        MTX6 = 0x54,

        // Brightness Control.
        BRIGHT = 0x55,

        // Contrast Control.
        CONTRAS = 0x56,

        // Contrast Center.
        CONTRAS_CENTER = 0x57,

        // Matrix Coefficient Sign for coefficient 5 to 0.
        MTXS = 0x58,

        // AWB Control 7.
        AWBC7 = 0x59,

        // AWB Control 8.
        AWBC8 = 0x5a,

        // AWB Control 9.
        AWBC9 = 0x5b,

        // AWB Control 10.
        AWBC10 = 0x5c,

        // AWB Control 11.
        AWBC11 = 0x5d,

        // AWB Control 12.
        AWBC12 = 0x5e,

        // AWB B Gain Range.
        B_LMT = 0x5f,

        // AWB R Gain Range.
        R_LMT = 0x60,

        // AWB G Gain Range.
        G_LMT = 0x61,

        // Lens Correction Option 1 - X Coordinate of Lens Correction Center Relative to Array Center.
        LCC1 = 0x62,

        // Lens Correction Option 2 - Y Coordinate of Lens Correction Center Relative to Array Center.
        LCC2 = 0x63,

        // Lens Correction Option 3
        LCC3 = 0x64,

        // Lens Correction Option 4 - Radius of the circular section where no compensation applies.
        LCC4 = 0x65,

        // Lens Correction Control.
        LCC5 = 0x66,

        // Manual U Value (effective only when register TSLB[4] is high).
        MANU = 0x67,

        // Manual V Value (effective only when register TSLB[4] is high).
        MANV = 0x68,

        // Fix gain control
        GFIX = 0x69,

        // G Channel AWB Gain.
        GGAIN = 0x6a,

        // PLL Control.
        DBLV = 0x6b,

        // AWB Control 3.
        AWBCTR3 = 0x6c,

        // AWB Control 2.
        AWBCTR2 = 0x6d,

        // AWB Control 1.
        AWBCTR1 = 0x6e,

        // AWB Control 0.
        AWBCTR0 = 0x6f,

        // Test_pattern[0] - works with test_pattern[1] test_pattern.
        SCALING_XSC = 0x70,

        // Test_pattern[1] - works with test_pattern[0] test_pattern (SCALING_XSC[7], SCALING_YSC[7]).
        SCALING_YSC = 0x71,

        // DCW Control
        SCALING_DCWCTR = 0x72,

        // Clock.
        SCALING_PCLK_DIV = 0x73,

        // Gain control.
        REG74 = 0x74,

        // Edge enhancement.
        REG75 = 0x75,

        // Pixel correction.
        REG76 = 0x76,

        // Offset, de-noise range control.
        REG77 = 0x77,

        // Gamma Curve Highest Segment Slop.
        SLOP = 0x7a,

        // Gamma Curve 1st Segment Input End Point 0x04 Output Value.
        GAM1 = 0x7b,

        // Gamma Curve 2nd Segment Input End Point 0x08 Output Value.
        GAM2 = 0x7c,

        // Gamma Curve 3rd Segment Input End Point 0x10 Output Value.
        GAM3 = 0x7d,

        // Gamma Curve 4th Segment Input End Point 0x20 Output Value.
        GAM4 = 0x7e,

        // Gamma Curve 5th Segment Input End Point 0x28 Output Value.
        GAM5 = 0x7f,

        // Gamma Curve 6th Segment Input End Point 0x30 Output Value.
        GAM6 = 0x80,

        // Gamma Curve 7th Segment Input End Point 0x38 Output Value.
        GAM7 = 0x81,

        // Gamma Curve 8th Segment Input End Point 0x40 Output Value.
        GAM8 = 0x82,

        // Gamma Curve 9th Segment Input End Point 0x48 Output Value.
        GAM9 = 0x83,

        // Gamma Curve 10th Segment Input End Point 0x50 Output Value.
        GAM10 = 0x84,

        // Gamma Curve 11th Segment Input End Point 0x60 Output Value.
        GAM11 = 0x85,

        // Gamma Curve 12th Segment Input End Point 0x70 Output Value.
        GAM12 = 0x86,

        // Gamma Curve 13th Segment Input End Point 0x90 Output Value.
        GAM13 = 0x87,

        // Gamma Curve 14th Segment Input End Point 0xB0 Output Value.
        GAM14 = 0x88,

        // Gamma Curve 15th Segment Input End Point 0xD0 Output Value.
        GAM15 = 0x89,

        // RGB 444 control
//        RGB444 = 0x8c,

// Dummy Row low 8 bits.
        DM_LNL = 0x92,

        // Dummy Row high 8 bits.
        DM_LNH = 0x93,

        // Lens Correction Option 6 (effective only when LCC5[2] is high).
        LCC6 = 0x94,

        // Lens Correction Option 7 (effective only when LCC5[2] is high).
        LCC7 = 0x95,

        // 50 Hz Banding Filter Value (effective only when COM8[5] is high and COM11[3] is high).
        BD50ST = 0x9d,

        // 60 Hz Banding Filter Value (effective only when COM8[5] is high and COM11[3] is low).
        BD60ST = 0x9e,

        // High Reference Luminance.
        HRL = 0x9f,

        // Low Reference Luminance.
        LRL = 0xa0,

        // DSP Control 3.
        DSPC3 = 0xa1,

        // DSP Control 3.
        SCALING_PCLK_DELAY = 0xa2,

        // Frame rate adjustment.
        NT_CTRL = 0xa4,

        // Maximum Banding Filter Step.
        AECGMAX = 0xa5,

        // Lower Limit of Probability for HRL, after exposure/gain stabilizes.
        LPH = 0xa6,

        // Upper Limit of Probability for LRL, after exposure/gain stabilizes.
        UPL = 0xa7,

        // Probability Threshold for LRL to control AEC/AGC speed.
        TPL = 0xa8,

        // Probability Threshold for HRL to control AEC/AGC speed.
        TPH = 0xa9,

        // AEC algorithm selection.
        NALG = 0xaa,

        // Strobe
        STR_OPT = 0xac,

        // Red gain for strobe.
        STR_R = 0xad,

        // Green gain for strobe.
        STR_G = 0xae,

        // Blue gain for strobe.
        STR_B = 0xaf,

        // ABLC function.
        ABLC1 = 0xb1,

        // ABLC Target.
        THL_ST = 0xb3,

        // ABLC Stable Range.
        THL_DLT = 0xb5,

        // Blue Channel Black Level Compensation.
        AD_CHB = 0xbe,

        // Red Channel Black Level Compensation.
        AD_CHR = 0xbf,

        // Gb Channel Black Level Compensation.
        AD_CHGB = 0xc0
    };

    enum FlashlightModeSelect {
        XENON = 0x00, LED1 = 0x01, LED2 = 0x02
    };

    enum OutputFormat {

        // YUV format
        YUV = 0x00,

        // RGB format
        RGB = 0x04,

        // Raw bayer RGB format
        RAW_BAYER_RGB = 0x01,

        // Processed bayer RGB format
        PROCESSED_BAYER_RGB = 0x05
    };

    enum OutputResolution {

        // VGA format.
        VGA = 0x00,

        // CIF format
        CIF = 0x20,

        // QVGA format
        QVGA = 0x10,

        // QCIF format
        QCIF = 0x08
    };

    enum RGBOutput {

        // Normal RGB
        RGB_NORMAL = 0x00,

        // RGB 565
        RGB_565 = 0x10,

        // RGB 555
        RGB_555 = 0x30
    };

    /**
     * Public constructor.
     *
     * @param read				The reader function.
     * @param vsyncPin			The vertical sync pin number.
     * @param hsyncPin			The horizontal sync pin number.
     * @param pclkPin			The clock pin number.
     *
     */
    CameraAL422B(unsigned char (*read)(), unsigned char vsyncPin,
            unsigned char writeEnPin, unsigned char readClockPin,
            unsigned char readResetPin);

    /**
     * Initializes the camera.
     */
    void begin();

    /**
     * Captures a frame.
     */
    virtual bool capture();

    /**
     * Returns a frame.
     *
     * @return               A frame.
     */
    int readFrame(OutputStream *out);

    /**
     * En/disable horizontal mirror.
     *
     * @param mirror        The mirror option.
     */
    void inline setHorizontalMirror(bool mirror);

    /**
     * En/disable vertical flip.
     *
     * @param mirror        The vertical flip.
     */
    void inline setVerticalFlip(bool flip);

    /**
     * Select the flashlight mode.
     *
     * @param mode          The FlashlightModeSelect to be used.
     */
    void inline setFlashlightModeSelect(FlashlightModeSelect mode);

    /**
     * Exit/Enter strobe mode.
     *
     * @param request       Enter or exit.
     */
    void inline setStrobeRequest(bool request);

    /**
     * Enable/Disable color gain.
     *
     * @param enable        Enable or disable.
     */
    void inline setColorGainControlEnable(bool enable);

    /**
     * Sets the output format.
     *
     * @param format            The output format.
     */
    void setOutputFormat(OutputFormat format);

    /**
     * Sets predefined output resolution.
     *
     * @param resolution        The output resolution.
     */
    void setOutputResolution(OutputResolution resolution);

    /**
     * Sets the RGB output.
     *
     * @param output            The RGB output.
     */
    void setRGBOutput(RGBOutput output);

    /**
     * Enables write to the FIFO.
     */
    void inline enableWrite();

    /**
     * Disables write to the FIFO.
     */
    void inline disableWrite();

    /**
     * Resets the FIFO internal read pointer.
     */
    void inline resetReadPointer();

    /**
     * Configures a registers inside the camera.
     *
     * @param reg           The register number.
     * @param mask          The mask to be used.
     * @param v             The value to be used.
     */
    void configureRegisterBits(Register reg, Mask mask, unsigned char v);

    /**
     * Writes a value to a register.
     *
     * @param reg           The register number.
     * @param v             The value to be used.
     */
    void writeRegister(Register reg, unsigned char v);

    /**
     * Reades a value from a register.
     *
     * @param reg           The register number.
     * @return              The register value.
     */
    unsigned char readRegister(Register reg);

private:

    /**
     * Resets all register to default value.
     */
    void inline resetRegisters();

    /**
     * Reads a row.
     *
     * @param out           The output stream to be read into.
     */
    int readRow(OutputStream *out);
};

#endif /* __ARDUINO_DRIVER_CAMERA_AL422B_H__ */
