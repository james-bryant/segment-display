#include <stdio.h>
#include <string.h>
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "pico/stdlib.h"

#include "segment_display.h"

static const uint16_t DIGITS[] = {
        // nmlkjihgfedcba
        0b00000000000000, // ' ' (space)
        0b00001000001000, // '!'
        0b00001000000010, // '"'
        0b01001101001110, // '//'
        0b01001101101101, // '$'
        0b10010000100100, // '%'
        0b00110011011001, // '&'
        0b00001000000000, // '''
        0b00000000111001, // '('
        0b00000000001111, // ')'
        0b11111010000000, // '*'
        0b01001101000000, // '+'
        0b10000000000000, // ','
        0b00000101000000, // '-'
        0b00000000000000, // '.'
        0b10010000000000, // '/'
        0b00000000111111, // '0'
        0b00010000000110, // '1'
        0b00000101011011, // '2'
        0b00000101001111, // '3'
        0b00000101100110, // '4'
        0b00000101101101, // '5'
        0b00000101111101, // '6'
        0b01010000000001, // '7'
        0b00000101111111, // '8'
        0b00000101100111, // '9'
        0b00000000000000, // ':'
        0b10001000000000, // ';'
        0b00110000000000, // '<'
        0b00000101001000, // '='
        0b01000010000000, // '>'
        0b01000100000011, // '?'
        0b00001100111011, // '@'
        0b00000101110111, // 'A'
        0b01001100001111, // 'B'
        0b00000000111001, // 'C'
        0b01001000001111, // 'D'
        0b00000101111001, // 'E'
        0b00000101110001, // 'F'
        0b00000100111101, // 'G'
        0b00000101110110, // 'H'
        0b01001000001001, // 'I'
        0b00000000011110, // 'J'
        0b00110001110000, // 'K'
        0b00000000111000, // 'L'
        0b00010010110110, // 'M'
        0b00100010110110, // 'N'
        0b00000000111111, // 'O'
        0b00000101110011, // 'P'
        0b00100000111111, // 'Q'
        0b00100101110011, // 'R'
        0b00000110001101, // 'S'
        0b01001000000001, // 'T'
        0b00000000111110, // 'U'
        0b10010000110000, // 'V'
        0b10100000110110, // 'W'
        0b10110010000000, // 'X'
        0b01010010000000, // 'Y'
        0b10010000001001, // 'Z'
        0b00000000111001, // '['
        0b00100010000000, // '\'
        0b00000000001111, // ']'
        0b10100000000000, // '^'
        0b00000000001000, // '_'
        0b00000010000000, // '`'
        0b00000101011111, // 'a'
        0b00100001111000, // 'b'
        0b00000101011000, // 'c'
        0b10000100001110, // 'd'
        0b00000001111001, // 'e'
        0b00000001110001, // 'f'
        0b00000110001111, // 'g'
        0b00000101110100, // 'h'
        0b01000000000000, // 'i'
        0b00000000001110, // 'j'
        0b01111000000000, // 'k'
        0b01001000000000, // 'l'
        0b01000101010100, // 'm'
        0b00100001010000, // 'n'
        0b00000101011100, // 'o'
        0b00010001110001, // 'p'
        0b00100101100011, // 'q'
        0b00000001010000, // 'r'
        0b00000110001101, // 's'
        0b00000001111000, // 't'
        0b00000000011100, // 'u'
        0b10000000010000, // 'v'
        0b10100000010100, // 'w'
        0b10110010000000, // 'x'
        0b00001100001110, // 'y'
        0b10010000001001, // 'z'
        0b10000011001001, // '{'
        0b01001000000000, // '|'
        0b00110100001001, // '}'
        0b00000101010010, // '~'
        0b11111111111111 // Unknown character (DEL or RUBOUT)
};

SegmentDisplay::SegmentDisplay(i2c_inst_t *i2c, uint8_t address)
{
    this->address = address;
    this->i2c = i2c;
    this->msgBuf.reg = 0x00; // Set register to 0x00
    memset(this->msgBuf.buffer, 0x00, 16); // Clear the buffer

    uint8_t rxdata;
    int ret = i2c_read_blocking(i2c, address, &rxdata, 1, false);
    if (ret <= 0) {
        printf("device at address 0x%02x NOT found (%d)\n", address, ret);
    } else {
        printf("device at address 0x%02x found\n", address);
    }
}

SegmentDisplay::~SegmentDisplay()
{
}

void SegmentDisplay::displayOn()
{
    uint8_t cmd = SEGMENT_DISPLAY_CMD_SYSTEM_SETUP | 0b1;
    while (i2c_write_blocking(i2c, address, &cmd, 1, false) <= 0) {
        printf("retrying i2c write command 0x%02x to 0x%02x\n", cmd, address);
    }

    cmd = SEGMENT_DISPLAY_CMD_DISPLAY_SETUP | 0b1;
     while (i2c_write_blocking(i2c, address, &cmd, 1, false) <= 0) {
        printf("retrying i2c write command 0x%02x to 0x%02x\n", cmd, address);
    }
}

void SegmentDisplay::clear()
{
    memset(this->msgBuf.buffer, 0x00, 16);
}

void SegmentDisplay::setChar(uint8_t digit, char c)
{
    uint8_t char_num = c - ' ';
    uint16_t segments = DIGITS[char_num];

    for (uint8_t i = 0; i < 14; i++) {
        this->setSegment(digit, i, segments >> i & 0x1 > 0);
    }
}

void SegmentDisplay::updateDisplay()
{
    // Send the entire message buffer including the register
    while (i2c_write_blocking(i2c, address, (uint8_t *)&msgBuf, sizeof(Message), false) <= 0) {
        printf("retrying i2c write data to 0x%02x\n", address);
    }
}

void SegmentDisplay::setString(char *str)
{
    int len = strlen(str);
    
    if (len > 4) {
        printf("warning: string longer than 4 characters.\n");
        len = 4;
    }

    for (uint8_t i = 0; i < len; i++) {
        this->setChar(i, str[i]);
    }
}

void SegmentDisplay::setDecimal(bool b)
{
    if (b)
        msgBuf.buffer[0x03] = msgBuf.buffer[0x03] | 0x01;
    else
        msgBuf.buffer[0x03] = msgBuf.buffer[0x03] & ~0x01;
}

void SegmentDisplay::setSegment(uint8_t digit, uint8_t segment, bool on)
{
	uint8_t com;
	uint8_t row;

	com = segment; // Convert the segment letter back to a number

	if (com > 6)
		com -= 7;
	if (segment == SEG_I)
		com = 0;
	if (segment == SEG_H)
		com = 1;

	row = digit;
	if (segment > SEG_G)
		row += 4;

	uint8_t adr = com * 2;

	// Determine the address
	if (row > 7)
		adr++;

	// Determine the data bit
	if (row > 7)
		row -= 8;
	uint8_t dat = 1 << row;

    if (on)
        msgBuf.buffer[adr] = msgBuf.buffer[adr] | dat;
    else
        msgBuf.buffer[adr] = msgBuf.buffer[adr] & ~dat;
}