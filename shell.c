#include "mcc_generated_files/eusart1.h"
#include "shell.h"
#include "S2LP_Config.h"
#include <string.h>
#include <stdio.h>
#include "timers.h"

__eeprom uint32_t p_F = 433000000; // frequency
__eeprom ModulationSelect p_M = MOD_2FSK; // modulation
__eeprom uint32_t p_R = 38400; // datarate
__eeprom uint32_t p_W = 100000; //bandwidth
__eeprom uint32_t p_D = 20000; // freq_deviation
__eeprom int32_t p_P = 14; // power

char pars[] = {
    'F', // frequency
    'M', //modulation
    'R', // datarate
    'W', // bandwith
    'D', // deviation
    'P', // Power
    0
};
uint8_t npars = 6;

extern volatile uint8_t eusart1RxCount;
char c_buf[BUF_LEN], val_buf[BUF_LEN];
uint8_t c_len;
char prompt[] = {'\r', '\n', '>', ' '};
char err[] = {'\r', '\n', 'E', 'r', 'r', 'o', 'r', '\r', '\n', '>', ' '};
char ex[] = {'\r', '\n', 'E', 'x', 'i', 't', '\r', '\n'};
char commands[] = {'S', 'L', 'D'};

void send_chars(char* x, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) EUSART1_Write(x[i]);
    while (!EUSART1_is_tx_done());
}

void empty_RXbuffer() {
    while (eusart1RxCount != 0) EUSART1_Read();
}

uint8_t stringToUInt32(char* str, uint32_t* val) //it is a function made to convert the string value to integer value.
{
    uint8_t i = 0;
    uint32_t sum = 0;
    while (str[i] != '\0') //string not equals to null
    {

        if (str[i] < 48 || str[i] > 57) return 1; // ascii value of numbers are between 48 and 57.
        else {
            sum = sum * 10 + (str[i] - 48);
            i++;
        }
    }
    *val = sum;
    return 0;
}

uint8_t stringToUInt8(char* str, uint8_t* val) //it is a function made to convert the string value to integer value.
{
    uint8_t i = 0;
    uint8_t sum = 0;
    while (str[i++] != 0);
    if (i > 3) return 1;
    if (i == 3) {
        if (str[0] > 0x32) return 1;
        if (str[0] == 0x32) {
            if (str[1] > 0x35) return 1;
            if (str[1] == 0x35 && str[2] > 0x35) return 1;
        }
    }
    i = 0;
    while (str[i] != '\0') //string not equals to null
    {
        if (str[i] < 48 || str[i] > 57) return 1; // ascii value of numbers are between 48 and 57.
        else {
            sum = sum * 10 + (str[i] - 48);
            i++;
        }
    }
    *val = sum;
    return 0;
}

uint8_t stringToInt32(char* str, int32_t* val) //it is a function made to convert the string value to integer value.
{
    uint8_t i = 0, sign = 0;
    int32_t sum = 0;
    if (str[0] == '-') {
        sign = 1;
        i = 1;
    }
    while (str[i] != '\0') //string not equals to null
    {

        if (str[i] < 48 || str[i] > 57) return 1; // ascii value of numbers are between 48 and 57.
        else {
            sum = sum * 10 + (str[i] - 48);
            i++;
        }
    }
    if (sign) *val = -sum;
    else *val = sum;
    return 0;
}

uint8_t set_par(char par, char* val_buf) {
    switch (par) {
        case 'F': // frequency
            if (stringToUInt32(val_buf, &p_F)) return 1;
            return 0;
        case 'M': //modulation
            if (stringToUInt8(val_buf, (uint8_t*) & p_M)) return 1;
            return 0;
        case 'R': // datarate
            if (stringToUInt32(val_buf, &p_R)) return 1;
            return 0;
        case 'W': // bandwith
            if (stringToUInt32(val_buf, &p_W)) return 1;
            return 0;
        case 'D': // deviation
            if (stringToUInt32(val_buf, &p_D)) return 1;
            return 0;
        case 'P': // Power
            if (stringToInt32(val_buf, &p_P)) return 1;
            return 0;
        default:
            return 1;
    }
}

char* i32toa(int32_t i, char* b) {
    char const digit[] = "0123456789";
    char* p = b;
    if (i < 0) {
        *p++ = '-';
        i *= -1;
    }
    int32_t shifter = i;
    do { //Move to where representation ends
        ++p;
        shifter = shifter / 10;
    } while (shifter);
    *p = '\0';
    do { //Move back, inserting digits as u go
        *--p = digit[i % 10];
        i = i / 10;
    } while (i);
    return b;
}

char* ui32toa(uint32_t i, char* b) {
    char const digit[] = "0123456789";
    char* p = b;
    uint32_t shifter = i;
    do { //Move to where representation ends
        ++p;
        shifter = shifter / 10;
    } while (shifter);
    *p = '\0';
    do { //Move back, inserting digits as u go
        *--p = digit[i % 10];
        i = i / 10;
    } while (i);
    return b;
}

char* ui8toa(uint8_t i, char* b) {
    char const digit[] = "0123456789";
    char* p = b;
    uint8_t shifter = i;
    do { //Move to where representation ends
        ++p;
        shifter = shifter / 10;
    } while (shifter);
    *p = '\0';
    do { //Move back, inserting digits as u go
        *--p = digit[i % 10];
        i = i / 10;
    } while (i);
    return b;
}

void print_par(char par) {
    uint8_t i;
    switch (par) {
        case 'F': // frequency
            ui32toa(p_F, val_buf);
            break;
        case 'M': //modulation
            ui8toa(p_M, val_buf);
            break;
        case 'R': // datarate
            ui32toa(p_R, val_buf);
            break;
        case 'W': // bandwith
            ui32toa(p_W, val_buf);
            break;
        case 'D': // deviation
            ui32toa(p_D, val_buf);
            break;
        case 'P': // Power
            i32toa(p_P, val_buf);
            break;
        default:
            send_error();
            return;
    }
    EUSART1_Write(par);
    EUSART1_Write('=');
    i = 0;
    while (val_buf[i]) {
        EUSART1_Write(val_buf[i++]);
        while (!EUSART1_is_tx_done());
    }
    EUSART1_Write('\r');
    EUSART1_Write('\n');
    while (!EUSART1_is_tx_done());
}

void print_pars() {
    uint8_t i = 0;
    while (pars[i]) print_par(pars[i++]);
}

uint8_t proceed() {
    uint8_t i = 1, par, val, cmd;
    //    printf("proceed %s\r\n",c_buf);
    c_buf[c_len] = 0;
    cmd = c_buf[0];
    if (cmd == 'Q' && c_buf[1] == 0) {
        send_exit();
        return 0;
    }
    if (cmd == 'L' && c_buf[1] == 0) {
        print_pars();
        return 1;
    }
    while (c_buf[i] == ' ' || c_buf[i] == '\t') i++;
    par = c_buf[i];
    uint8_t ip = 0, ip0 = 0xff;
    do {
        if (pars[ip] == par) {
            ip0 = ip;
            break;
        }
    } while (pars[++ip]);
    if (ip0 == 0xff) return 2;
    if (cmd == 'D') {
        if (c_buf[i + 1] == 0) {
            print_par(par);
            return 1;
        } else return 2;
    }
    i++;
    while (c_buf[i] == ' ' || c_buf[i] == '\t') i++;
    if (c_buf[i++] != '=') return 2;
    while (c_buf[i] == ' ' || c_buf[i] == '\t') i++;
    ip = 0;
    do {
        val_buf[ip++] = c_buf[i];
    } while (c_buf[i++]);
    if (set_par(par, val_buf)) return 2;
    print_par(par);
    return 1;
}

void start_x_shell(void) {
    char c, cmd, par;
    uint8_t start = 0;
    //    printf("Start shell\r");

    c_len = 0;
    SetTimer3(11000);
    send_prompt();
    while (1) {
        if (!start) {
            if (TestTimer3()) {
                send_exit();
                return;
            }
        }
        if (eusart1RxCount != 0) {
            c = EUSART1_Read();
            EUSART1_Write(c);
            if (c == 0x08) {
                EUSART1_Write(' ');
                EUSART1_Write(c);
                c_len--;
                while (!EUSART1_is_tx_done());
                continue;
            }
            while (!EUSART1_is_tx_done());
            start = 1;
            switch (c) {
                case '\r':
                case '\n':
                    c_buf[c_len] = 0;
                    empty_RXbuffer();
                    uint8_t r = proceed();
                    if (r == 0) return;
                    if (r != 1) send_error()
                    else send_prompt();
                    break;
                default:
                    if (c >= 0x61 && c <= 0x7A) c -= 0x20;
                    c_buf[c_len++] = c;
                    continue;
            }
            empty_RXbuffer();
            c_len = 0;
        }
    }
}
