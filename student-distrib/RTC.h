#ifndef _RTC_H
#define _RTC_H

#include "terminal_switch.h"

#define REG_A                      0xA                  // RTC Register A port
#define REG_B                      0xB                  // RTC Register B port
#define REG_C                      0xC                  // RTC Register C port

#define STATUS_REG_A               0x8A                 // Status port of Register A
#define STATUS_REG_B               0x8B                 // Status port of Register B
#define STATUS_REG_C               0x8C                 // Status port of Register C

#define IND_PORT                   0x70                 // Index port
#define IND_DATA_PORT              0x71                 // Data port

#define RTC_PIC_port               8                    // RTC PIC port
#define max_freq                   32768                // 2^15 the maximum built in rate for the rtc


// initiales the RTC
void init_RTC();


int rtc_read(); //Returns return 0 on RTC interrupt
int rtc_write(int freq); //DESCRIPTION: records frequency for the RTC of this program
int rtc_write_sched(int freq);  //DESCRIPTION: Changes frequency of the RTC
int rtc_open(); // DESCRIPTION: initializes RTC to 2HZ, return 0
int rtc_close(); // Nothing For Now

int interrupt[MAX_TERMINAL_COUNT];


#endif
