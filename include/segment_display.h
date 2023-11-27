#ifndef __SEGMENT_DISPLAY_H__
#define __SEGMENT_DISPLAY_H__

#include "hardware/i2c.h"

#define SEGMENT_DISPLAY_BLINK_RATE_NOBLINK 0b00
#define SEGMENT_DISPLAY_BLINK_RATE_2HZ 0b01
#define SEGMENT_DISPLAY_BLINK_RATE_1HZ 0b10
#define SEGMENT_DISPLAY_BLINK_RATE_0_5HZ 0b11

#define SEGMENT_DISPLAY_CMD_SYSTEM_SETUP 0b00100000
#define SEGMENT_DISPLAY_CMD_DISPLAY_SETUP 0b10000000
#define SEGMENT_DISPLAY_CMD_DIMMING_SETUP 0b11100000

#define UNKNOWN_CHAR 95

#define SEG_G ('G' - 'A')
#define SEG_H ('H' - 'A')
#define SEG_I ('I' - 'A')

class SegmentDisplay
{
private:
    typedef struct {
        uint8_t reg;
        uint8_t buffer[16];
    } Message;

    i2c_inst_t *i2c;
    uint8_t address;
    Message msgBuf;

public:
    SegmentDisplay(i2c_inst_t *i2c, uint8_t address);
    ~SegmentDisplay();
    
    void displayOn();
    void updateDisplay();
    void clear();
    void setChar(uint8_t digit, char c);
    void setString(char *str);
    void setDecimal(bool b);
    void setSegment(uint8_t digit, uint8_t segment, bool on);
};

#endif // __SEGMENT_DISPLAY_H__