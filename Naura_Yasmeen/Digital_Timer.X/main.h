#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#include <stdio.h>
#include "clcd.h"
#include "digital_keypad.h"
#include "ds1307.h"
#include "i2c.h"
#include "timers.h"
#include "digital_timer.h"

#define DEFAULT_SCREEN      0x01
#define CONFIG_SCREEN       0x02
#define SET_TIME_SCREEN     0x03
#define SET_ALARM_SCREEN    0x04
#define ADD_ALARM_SCREEN    0x05
#define EDIT_ALARM_SCREEN   0x06

#define RESET_TIME_DATE     0xF1
#define RESET_VIEW_ALARM    0xF2
#define RESET_EDIT_ALARM    0xF3
#define RESET_NOTHING       0xF4

#define MAX_ALARMS          8

#define ONCE                'O'
#define DAILY               'D'
#define WEEKLY              'W'

#define ADD_ALARM_MODE      0xA1
#define EDIT_ALARM_MODE     0xB1

typedef struct
{
    unsigned char hr;
    unsigned char min;
    char type;     // daily(D) / weekly (W) / once (O)
}Alarm;

#endif	/* MAIN_H */

