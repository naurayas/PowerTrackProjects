#ifndef DIGITAL_TIMER_H
#define	DIGITAL_TIMER_H

void default_screen(void);
void display_config_menu(unsigned char key, unsigned char *screen_flag, unsigned char *reset_flag);
void set_time_date(unsigned char *key, unsigned char *screen_flag, unsigned char *reset_flag);
void set_view_alarm(unsigned char *key, unsigned char *screen_flag, unsigned char *reset_flag);
void add_edit_alarm(unsigned char *key, unsigned char *screen_flag, unsigned char *reset_flag);

#endif	/* DIGITAL_TIMER_H */

