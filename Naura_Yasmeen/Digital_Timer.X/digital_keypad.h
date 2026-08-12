
#ifndef DIGITAL_KEYPAD_H
#define	DIGITAL_KEYPAD_H

#define LEVEL_DETECTION         0
#define LEVEL                   0

#define STATE_DETECTION         1
#define STATE                   1

#define NO_KEY                  0x03

#define KEYPAD_PORT             PORTB
#define KEYPAD_PORT_DDR         TRISB

#define INPUT_LINES             0x3F

#define SW1_SP                  0x3E
#define SW2_SP                  0x3D
#define SW3_SP                  0x3B
#define SW4_SP                  0x37
#define SW5_SP                  0x2F

#define SW1_LP                  0xFE
#define SW2_LP                  0xFD
#define SW3_LP                  0xFB
#define SW4_LP                  0xF7
#define SW5_LP                  0xFF

#define ALL_RELEASED            INPUT_LINES

unsigned char read_digital_keypad(unsigned char mode);
void init_digital_keypad(void);

#endif	/* DIGITAL_KEYPAD_H */

