#include "main.h"

// strings to get the date and time from RTC
unsigned char clock_reg[6];
char date[9];
char time[7];
char noon;

char str[8];

static unsigned char hr, min, sec, type;
static unsigned char field, blink_flag;
static unsigned int delay;
static unsigned char dd, mm, yy;

// idle state tracker
volatile char return_time;

Alarm alarms[MAX_ALARMS];
static unsigned char alarm_count;
static unsigned char alarm_pos;
unsigned char edit_mode;

static void get_time(void) {
    clock_reg[0] = read_ds1307(DATE_ADDR); // DD -> BCD
    clock_reg[1] = read_ds1307(MONTH_ADDR); // MM -> BCD
    clock_reg[2] = read_ds1307(YEAR_ADDR); // YY -> BCD
    clock_reg[3] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[4] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[5] = read_ds1307(SEC_ADDR); // SS -> BCD

    // BCD to ASCII
    // Date
    date[0] = ((clock_reg[0] >> 4) & 0x0F) + '0';
    date[1] = (clock_reg[0] & 0x0F) + '0';

    // Month
    date[2] = ((clock_reg[1] >> 4) & 0x0F) + '0';
    date[3] = (clock_reg[1] & 0x0F) + '0';

    // year
    date[4] = '2';
    date[5] = '0';
    date[6] = ((clock_reg[2] >> 4) & 0x0F) + '0';
    date[7] = (clock_reg[2] & 0x0F) + '0';
    date[8] = '\0';

    // hour
    time[0] = ((clock_reg[3] >> 4) & 0x0F) + '0';
    time[1] = (clock_reg[3] & 0x0F) + '0';

    // minute 
    time[2] = ((clock_reg[4] >> 4) & 0x0F) + '0';
    time[3] = (clock_reg[4] & 0x0F) + '0';

    // second
    time[4] = ((clock_reg[5] >> 4) & 0x0F) + '0';
    time[5] = (clock_reg[5] & 0x0F) + '0';
    time[6] = '\0';
}

static void extract_time_date(void) {
    get_time();
    // extract date
    dd = ((clock_reg[0] >> 4) & 0x0F);
    dd = dd * 10 + (clock_reg[0] & 0x0F);

    mm = ((clock_reg[1] >> 4) & 0x0F);
    mm = mm * 10 + (clock_reg[1] & 0x0F);

    yy = ((clock_reg[2] >> 4) & 0x0F);
    yy = yy * 10 + (clock_reg[2] & 0x0F);

    // extract time
    hr = ((clock_reg[3] >> 4) & 0x0F);
    hr = hr * 10 + (clock_reg[3] & 0x0F);

    min = ((clock_reg[4] >> 4) & 0x0F);
    min = min * 10 + (clock_reg[4] & 0x0F);

    sec = ((clock_reg[5] >> 4) & 0x0F);
    sec = sec * 10 + (clock_reg[5] & 0x0F);

    field = 0;
    blink_flag = 0;
    delay = 5;
}

static int get_next_alarm(void)
{
    int cur_time, alarm_time, diff;
    int min_diff = 1441;
    int next_alarm = -1;
    
    unsigned char i;
    get_time();
    hr  = ((clock_reg[3] >> 4) * 10) + (clock_reg[3] & 0x0F);
    min = ((clock_reg[4] >> 4) * 10) + (clock_reg[4] & 0x0F); 
    cur_time = (hr * 60) + min;
    for(i = 0; i < alarm_count; i++)
    {
        alarm_time = (alarms[i].hr * 60) + alarms[i].min;
        diff = alarm_time - cur_time;
        
        if(diff < 0)
        {
            if(alarms[i].type == DAILY)
                diff += 1440;   // tomorrow
            else
                continue;
        }
        if(diff < min_diff)
        {
            min_diff = diff;
            next_alarm = i;
        }
    }
    return next_alarm;
}

static void display_next_event(void)
{
    int index = get_next_alarm();
    if(index == -1)
    {
        clcd_print("          ", LINE4(7));
        return;
    }
    clcd_putch((alarms[index].hr / 10) + '0', LINE4(7));
    clcd_putch((alarms[index].hr % 10) + '0', LINE4(8));
    clcd_putch(':', LINE4(9));
    clcd_putch((alarms[index].min / 10) + '0', LINE4(10));
    clcd_putch((alarms[index].min % 10) + '0', LINE4(11));
    clcd_putch((alarms[index].type), LINE4(13));
}

void default_screen(void) {
    // display date
    clcd_print("DATE: ", LINE1(0));
    get_time();
    clcd_putch(date[0], LINE1(6));
    clcd_putch(date[1], LINE1(7));
    clcd_putch('/', LINE1(8));
    clcd_putch(date[2], LINE1(9));
    clcd_putch(date[3], LINE1(10));
    clcd_putch('/', LINE1(11));
    clcd_putch(date[4], LINE1(12));
    clcd_putch(date[5], LINE1(13));
    clcd_putch(date[6], LINE1(14));
    clcd_putch(date[7], LINE1(15));

    // display time
    clcd_print("TIME: ", LINE2(0));
    clcd_putch(time[0], LINE2(6));
    clcd_putch(time[1], LINE2(7));
    clcd_putch(':', LINE2(8));
    clcd_putch(time[2], LINE2(9));
    clcd_putch(time[3], LINE2(10));
    clcd_putch(':', LINE2(11));
    clcd_putch(time[4], LINE2(12));
    clcd_putch(time[5], LINE2(13));

    noon = ((time[0] - '0') * 10) + (time[1] - '0');
    if (noon >= 12) {
        clcd_print("PM", LINE2(14));
    } else {
        clcd_print("AM", LINE2(14));
    }
    //clcd_putch(alarm_pos + '0', LINE3(0));
    //clcd_putch(alarm_count + '0', LINE3(5));

    // display next event
    clcd_print("EVENT: ", LINE4(0));
    display_next_event();
}

void display_config_menu(unsigned char key, unsigned char *screen_flag, unsigned char *reset_flag) {
    // config menu
    static char *menu[] = {"Set Time/Date", "Set/View Alarm"};
    static char menu_pos = 0;

    // return if idle for 5s
    if (return_time == 0) {
        clear_screen();
        // change to default screen at 5s timeout when idle
        *screen_flag = DEFAULT_SCREEN;
        TMR2ON = 0;
        return;
    }

    // navigate through menu
    if (key == SW1_SP && menu_pos < 1) {
        menu_pos++;
        clear_screen();
        return_time = 5;
    } else if (key == SW2_SP && menu_pos > 0) {
        menu_pos--;
        clear_screen();
        return_time = 5;
    }

    // display * for current menu position
    if (menu_pos == 0) {
        clcd_putch('*', LINE1(0));
    } else {
        clcd_putch('*', LINE2(0));
    }
    clcd_print(menu[0], LINE1(2));
    clcd_print(menu[1], LINE2(2));

    // SW2 long press to select the menu
    if (key == SW2_LP) {
        return_time = 5;
        if (menu_pos == 0) {
            *screen_flag = SET_TIME_SCREEN;
            *reset_flag = RESET_TIME_DATE;
            clear_screen();
            return;
        } else if (menu_pos == 1) {
            *screen_flag = SET_ALARM_SCREEN;
            *reset_flag = RESET_VIEW_ALARM;
            clear_screen();
            return;
        }
    }
}

static void save_time_date(void) {
    dd = (unsigned char) (((dd / 10) << 4) | (dd % 10));
    mm = (unsigned char) (((mm / 10) << 4) | (mm % 10));
    yy = (unsigned char) (((yy / 10) << 4) | (yy % 10));

    hr = (unsigned char) (((hr / 10) << 4) | (hr % 10));
    min = (unsigned char) (((min / 10) << 4) | (min % 10));
    sec = (unsigned char) (((sec / 10) << 4) | (sec % 10));

    write_ds1307(DATE_ADDR, dd);
    write_ds1307(MONTH_ADDR, mm);
    write_ds1307(YEAR_ADDR, yy);

    write_ds1307(HOUR_ADDR, hr);
    write_ds1307(MIN_ADDR, min);
    write_ds1307(SEC_ADDR, sec);
}

void set_time_date(unsigned char *key, unsigned char *screen_flag, unsigned char *reset_flag) {
    //clcd_print("time & date", LINE1(0));
    if (*reset_flag == RESET_TIME_DATE) {
        extract_time_date();
        *key = ALL_RELEASED;
        *reset_flag = RESET_NOTHING;
        delay = 5;
        field = 0;
        blink_flag = 0;
        clcd_print("DD/MM/YYYY", LINE1(3));
        clcd_print("HH:MM:SS", LINE3(4));
    }

    // SW2 sp to increment time
    if (*key == SW2_SP) {
        if (field == 0) {
            dd++;
            if (dd > 31)
                dd = 1;
        } else if (field == 1) {
            mm++;
            if (mm > 12)
                mm = 1;
        } else if (field == 2) {
            yy++;
            if (yy > 99)
                yy = 0;
        } else if (field == 5) {
            sec++;
            if (sec >= 60)
                sec = 0;
        } else if (field == 4) {
            min++;
            if (min >= 60)
                min = 0;
        } else if (field == 3) {
            hr++;
            if (hr >= 24)
                hr = 0;
        }
        blink_flag = 0;
        delay = 5;
    }        // SW1 sp to move field
    else if (*key == SW1_SP) {
        if (field < 5)
            field++;
        else
            field = 0;
        blink_flag = 0;
        delay = 5;
    }        // SW2 lp to save and return to config screen
    else if (*key == SW2_LP) {
        save_time_date();
        clear_screen();
        clcd_print("Time / Date set", LINE2(0));
        clcd_print("Successful", LINE3(0));
        __delay_ms(2000);
        clear_screen();
        *screen_flag = CONFIG_SCREEN;
        return_time = 5;
        TMR2ON = 1;
        *reset_flag = RESET_TIME_DATE;
        return;
    }

    // blink seconds field
    if (field == 5) {
        clcd_putch((hr / 10) + '0', LINE4(4));
        clcd_putch((hr % 10) + '0', LINE4(5));
        clcd_putch(':', LINE4(6));
        clcd_putch((min / 10) + '0', LINE4(7));
        clcd_putch((min % 10) + '0', LINE4(8));
        clcd_putch(':', LINE4(9));

        if (--delay == 0) {
            delay = 5;
            if (blink_flag == 0) {
                clcd_putch((sec / 10) + '0', LINE4(10));
                clcd_putch((sec % 10) + '0', LINE4(11));
                blink_flag = 1;
            } else {
                clcd_print("  ", LINE4(10));
                blink_flag = 0;
            }
        }
    }        // blink minutes field
    else if (field == 4) {
        clcd_putch((hr / 10) + '0', LINE4(4));
        clcd_putch((hr % 10) + '0', LINE4(5));
        clcd_putch(':', LINE4(6));
        clcd_putch(':', LINE4(9));
        clcd_putch((sec / 10) + '0', LINE4(10));
        clcd_putch((sec % 10) + '0', LINE4(11));

        if (--delay == 0) {
            delay = 5;
            if (blink_flag == 0) {
                clcd_putch((min / 10) + '0', LINE4(7));
                clcd_putch((min % 10) + '0', LINE4(8));
                blink_flag = 1;
            } else {
                clcd_print("  ", LINE4(7));
                blink_flag = 0;
            }
        }
    }        // blink hours field
    else if (field == 3) {
        clcd_putch(':', LINE4(6));
        clcd_putch((min / 10) + '0', LINE4(7));
        clcd_putch((min % 10) + '0', LINE4(8));
        clcd_putch(':', LINE4(9));
        clcd_putch((sec / 10) + '0', LINE4(10));
        clcd_putch((sec % 10) + '0', LINE4(11));

        if (--delay == 0) {
            delay = 5;
            if (blink_flag == 0) {
                clcd_putch((hr / 10) + '0', LINE4(4));
                clcd_putch((hr % 10) + '0', LINE4(5));
                blink_flag = 1;
            } else {
                clcd_print("  ", LINE4(4));
                blink_flag = 0;
            }
        }
    }        // blink year field
    else if (field == 2) {
        clcd_putch((dd / 10) + '0', LINE2(3));
        clcd_putch((dd % 10) + '0', LINE2(4));
        clcd_putch('/', LINE2(5));
        clcd_putch((mm / 10) + '0', LINE2(6));
        clcd_putch((mm % 10) + '0', LINE2(7));
        clcd_putch('/', LINE2(8));

        if (--delay == 0) {
            delay = 5;
            if (blink_flag == 0) {
                clcd_print("20", LINE2(9));
                clcd_putch((yy / 10) + '0', LINE2(11));
                clcd_putch((yy % 10) + '0', LINE2(12));
                blink_flag = 1;
            } else {
                clcd_print("    ", LINE2(9));
                blink_flag = 0;
            }
        }
    }        // blink month field
    else if (field == 1) {
        clcd_putch((dd / 10) + '0', LINE2(3));
        clcd_putch((dd % 10) + '0', LINE2(4));
        clcd_putch('/', LINE2(5));
        clcd_putch('/', LINE2(8));
        clcd_print("20", LINE2(9));
        clcd_putch((yy / 10) + '0', LINE2(11));
        clcd_putch((yy % 10) + '0', LINE2(12));

        if (--delay == 0) {
            delay = 5;
            if (blink_flag == 0) {
                clcd_putch((mm / 10) + '0', LINE2(6));
                clcd_putch((mm % 10) + '0', LINE2(7));
                blink_flag = 1;
            } else {
                clcd_print("  ", LINE2(6));
                blink_flag = 0;
            }
        }
    }        // blink date field
    else if (field == 0) {
        clcd_putch('/', LINE2(5));
        clcd_putch((mm / 10) + '0', LINE2(6));
        clcd_putch((mm % 10) + '0', LINE2(7));
        clcd_putch('/', LINE2(8));
        clcd_print("20", LINE2(9));
        clcd_putch((yy / 10) + '0', LINE2(11));
        clcd_putch((yy % 10) + '0', LINE2(12));

        if (--delay == 0) {
            delay = 5;
            if (blink_flag == 0) {
                clcd_putch((dd / 10) + '0', LINE2(3));
                clcd_putch((dd % 10) + '0', LINE2(4));
                blink_flag = 1;
            } else {
                clcd_print("  ", LINE2(3));
                blink_flag = 0;
            }
        }
    }
}

static void save_alarm(void) {
    if (edit_mode == ADD_ALARM_MODE) {
        if (alarm_count < MAX_ALARMS) {
            alarms[alarm_count].hr = hr;
            alarms[alarm_count].min = min;
            alarms[alarm_count].type = type;
            alarm_count++;
        }
    } else {
        alarms[alarm_pos].hr = hr;
        alarms[alarm_pos].min = min;
        alarms[alarm_pos].type = type;
    }
}

void add_edit_alarm(unsigned char *key, unsigned char *screen_flag, unsigned char *reset_flag) {
    if (*reset_flag == RESET_EDIT_ALARM) {
        *key = ALL_RELEASED;
        clcd_print("HH:MM O/D/W", LINE2(2));
        *reset_flag = RESET_NOTHING;
        field = 0;
        delay = 5;
        blink_flag = 0;
    }

    // SW2 sp to increment time
    if (*key == SW2_SP) {
        if (field == 2) {
            if (type == 'O')
                type = 'D';
            else if (type == 'D')
                type = 'W';
            else if (type == 'W')
                type = 'O';
        } else if (field == 1) {
            min++;
            if (min >= 60)
                min = 0;
        } else if (field == 0) {
            hr++;
            if (hr >= 24)
                hr = 0;
        }
        blink_flag = 0;
    }        
    // SW1 sp to move field
    else if (*key == SW1_SP) {
        if (field < 2)
            field++;
        else
            field = 0;
    }        
    // SW2 lp to save and return to config screen
    else if (*key == SW2_LP) {
        save_alarm();
        clear_screen();
        clcd_print("Add Alarm", LINE2(0));
        clcd_print("Successful", LINE3(0));
        __delay_ms(2000);
        clear_screen();
        *screen_flag = SET_ALARM_SCREEN;
        return_time = 5;
        TMR2ON = 1;
        *reset_flag = RESET_VIEW_ALARM;
        return;
    }

    // blink type field
    if (field == 2) {
        clcd_putch((hr / 10) + '0', LINE3(2));
        clcd_putch((hr % 10) + '0', LINE3(3));
        clcd_putch(':', LINE3(4));
        clcd_putch((min / 10) + '0', LINE3(5));
        clcd_putch((min % 10) + '0', LINE3(6));

        if (--delay == 0) {
            delay = 5;
            if (blink_flag == 0) {
                clcd_putch(type, LINE3(8));
                blink_flag = 1;
            } else {
                clcd_putch(' ', LINE3(8));
                blink_flag = 0;
            }
        }
    }        
    // blink minutes field
    else if (field == 1) {
        clcd_putch((hr / 10) + '0', LINE3(2));
        clcd_putch((hr % 10) + '0', LINE3(3));
        clcd_putch(':', LINE3(4));
        clcd_putch(type, LINE3(8));

        if (--delay == 0) {
            delay = 5;
            if (blink_flag == 0) {
                clcd_putch((min / 10) + '0', LINE3(5));
                clcd_putch((min % 10) + '0', LINE3(6));
                blink_flag = 1;
            } else {
                clcd_print("  ", LINE3(5));
                blink_flag = 0;
            }
        }
    }        
    // blink hours field
    else if (field == 0) {
        clcd_putch(':', LINE3(4));
        clcd_putch((min / 10) + '0', LINE3(5));
        clcd_putch((min % 10) + '0', LINE3(6));
        clcd_putch(type, LINE3(8));

        if (--delay == 0) {
            delay = 5;
            if (blink_flag == 0) {
                clcd_putch((hr / 10) + '0', LINE3(2));
                clcd_putch((hr % 10) + '0', LINE3(3));
                blink_flag = 1;
            } else {
                clcd_print("  ", LINE3(4));
                blink_flag = 0;
            }
        }
    }
}

static void delete_alarm(unsigned char index) {
    unsigned char i;
    for (i = index; i < alarm_count - 1; i++) {
        alarms[i] = alarms[i + 1];
    }
    alarm_count--;

    if (alarm_pos >= alarm_count && alarm_pos > 0) {
        alarm_pos--;
    }
}

void set_view_alarm(unsigned char *key, unsigned char *screen_flag, unsigned char *reset_flag) {
    //clcd_print("set / view alarm", LINE1(0));
    if (*reset_flag == RESET_VIEW_ALARM) {
        *key = ALL_RELEASED;
        *reset_flag = RESET_NOTHING;
        return_time = 5;
        alarm_pos = 0;
    }

    if (return_time == 0) {
        clear_screen();
        *screen_flag = CONFIG_SCREEN;
        return_time = 5;
        return;
    }

    if (alarm_pos == alarm_count) {
        clcd_print("* Add Alarm", LINE1(0));
        clcd_print("                ", LINE2(0));
    } else {
        clcd_print("* Alarm ", LINE1(0));
        clcd_putch((alarm_pos + 1) + '0', LINE1(8));

        clcd_putch((alarms[alarm_pos].hr / 10) + '0', LINE2(2));
        clcd_putch((alarms[alarm_pos].hr % 10) + '0', LINE2(3));
        clcd_putch(':', LINE2(4));
        clcd_putch((alarms[alarm_pos].min / 10) + '0', LINE2(5));
        clcd_putch((alarms[alarm_pos].min % 10) + '0', LINE2(6));
        clcd_putch((alarms[alarm_pos].type), LINE2(8));
    }

    // SW1 sp for moving down
    if (*key == SW1_SP) {
        if (alarm_pos < alarm_count) {
            alarm_pos++;
            clear_screen();
        }
        return_time = 5;
    }        // SW2 sp for moving up
    else if (*key == SW2_SP) {
        if (alarm_pos > 0) {
            alarm_pos--;
            clear_screen();
        }
        return_time = 5;
    }        // SW2 lp to select alarm and edit/add
    else if (*key == SW2_LP) {
        if (alarm_pos == alarm_count) {
            // add alarm
            edit_mode = ADD_ALARM_MODE;

            hr = 0;
            min = 0;
            type = DAILY;
        } else {
            // edit alarm
            edit_mode = EDIT_ALARM_MODE;

            hr = alarms[alarm_pos].hr;
            min = alarms[alarm_pos].min;
            type = alarms[alarm_pos].type;
        }
        *screen_flag = EDIT_ALARM_SCREEN;
        *reset_flag = RESET_EDIT_ALARM;
        TMR2ON = 0;
        return_time = 5;
        clear_screen();
    }        // SW1 (lp) to delete alarm
    else if (*key == SW1_LP) {
        delete_alarm(alarm_pos);
        clear_screen();
        clcd_print("Alarm Deleted", LINE1(0));
        clcd_print("Successfully", LINE2(0));
        __delay_ms(2000);
        clear_screen();
        *screen_flag = SET_ALARM_SCREEN;
        *reset_flag = RESET_VIEW_ALARM;
        return_time = 5;
    }
}
