/*
 * Name         : Naura Yasmeen U
 * Start Date   : 12 July, 2026
 * End Date     : 15 July, 2026
 * Project Name : Digital Timer
 * Usage        : Default Screen:
 *                  SW1/SW2 (DOWN/UP) (sp) - move to config screen
 *                Config Screen:
 *                  SW1 (sp) -  Move down
 *                  SW2 (sp) -  Move up
 *                  SW2 (lp) -  Select from menu  
 * 
 *                  If 5s idle, screen goes back to Defult screen
 *                Set time & date screen:
 *                  SW1 (sp) -  change fields
 *                  SW2 (sp) -  increment
 *                  SW2 (lp) -  save and back to config screen
 *                Set/View alarm screen:
 *                  SW1 (sp) - move down
 *                  SW2 (sp) - move up
 *                  SW1 (lp) - delete alarm
 *                  SW2 (lp) - select alarm to edit
 * 
 *                  If 5s idle, screen goes back to Config screen
 *                Edit Alarm screen:
 *                  SW1 (sp) - change fields
 *                  SW2 (sp) - increment
 *                  SW2 (lp) - save and back to set/view alarm screen
 * 
 */

#include "main.h"

// turn off WDT
#pragma config WDTE = OFF

extern volatile char return_time;

static void init_config(void) {
    // init i2c
    init_i2c(100000);
    // init rtc
    init_ds1307();
    // init clcd
    init_clcd();
    // init dkp
    init_digital_keypad();
    // init timer2
    init_timer2();
    
    // enable interrupts
    PEIE = 1;
    GIE = 1;
}

void main(void) {
    
    unsigned char screen_flag = DEFAULT_SCREEN;
    unsigned char reset_flag = RESET_NOTHING;
    
    unsigned char key;
    static unsigned char prev_key = ALL_RELEASED;
    static unsigned int press_time = 0;
    
    // initialize peripherals
    init_config();
    
    while (1) {
        
        // read key
        key = read_digital_keypad(LEVEL);
        
        if(key != ALL_RELEASED)
        {
            if(key == prev_key)
                press_time++;
            else
            {
                prev_key = key;
                press_time = 0;
            }
            key = NO_KEY;
        }
        else
        {
            if(prev_key != ALL_RELEASED)
            {
                if(press_time >= 5)
                {
                    switch(prev_key)
                    {
                        case SW1_SP:
                            key = SW1_LP;
                            break;
                        case SW2_SP:
                            key = SW2_LP;
                            break;
                        case SW3_SP:
                            key = SW3_LP;
                            break;
                        case SW4_SP:
                            key = SW4_LP;
                            break;
                        case SW5_SP:
                            key = SW5_LP;
                            break;
                    }
                }
                else
                {
                    key = prev_key;
                }
                prev_key = ALL_RELEASED;
                press_time = 0;
            }
            else
            {
                key = NO_KEY;
            }
        }
        
        // go to config screen for UP/DOWN sp
        if(screen_flag == DEFAULT_SCREEN && (key == SW1_SP || key == SW2_SP))
        {
            screen_flag = CONFIG_SCREEN;
            clear_screen();
            return_time = 5;
            // turn on timer
            TMR2ON = 1;
        }
        
        switch(screen_flag)
        {
            case DEFAULT_SCREEN:
                default_screen();
                break;
            case CONFIG_SCREEN:
                display_config_menu(key, &screen_flag, &reset_flag);
                break;
            case SET_TIME_SCREEN:
                TMR2ON = 0;
                set_time_date(&key, &screen_flag, &reset_flag);
                break;
            case SET_ALARM_SCREEN:
                TMR2ON = 1;
                set_view_alarm(&key, &screen_flag, &reset_flag);
                break;
            case EDIT_ALARM_SCREEN:
                add_edit_alarm(&key, &screen_flag, &reset_flag);
                break;
        }
    }
}
