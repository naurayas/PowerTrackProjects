# Digital Timer
This project implements a Digital Timer and Alarm Clock using the PIC16F877A microcontroller. It provides real-time clock functionality with time and date display, alarm configuration, countdown timer, and user interaction through a digital keypad and CLCD.

The project demonstrates embedded system concepts including peripheral interfacing, interrupt handling, real-time clock communication, and menu-driven user interfaces.

## Features
- Real-time clock using DS1307 RTC
- Displays current time and date
- Set time and date
- Configure alarm
- User input through digital keypad
- CLCD display interface

## Functionalities
### Real-Time CLock
- Reads current time from DS1307 RTC
- Displays time in HH:MM:SS 
- Displays current date

### Set Time and Date
- Modify hours, minutes, seconds
- Modify day, month, and year
- Updated values are written back to the RTC

### Digital Timer
- Starts countdown from user-defined value


## Peripherals
- GPIO
- Timer 2
- I2C
- Interrupts
- CLCD
- Digital Keypad
- DS1307 RTC

## Project Structure
```bash
.
├── Makefile
├── clcd.c
├── clcd.h
├── digital_keypad.c
├── digital_keypad.h
├── digital_timer.c
├── digital_timer.h
├── ds1307.c
├── ds1307.h
├── i2c.c
├── i2c.h
├── isr.c
├── main.c
├── main.h
├── timers.c
└── timers.h

```

## Build Instructions
1. Open the project in MPLAB X IDE
2. Build the project
3. The generated HEX files can be used in the PIC16F877A microcontroller

