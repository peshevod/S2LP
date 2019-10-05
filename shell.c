#include "mcc_generated_files/eusart1.h"
#include "shell.h"
#include "S2LP_Config.h"
#include <string.h>
#include <stdio.h>
#include "timers.h"

__eeprom _par _pars[]={
    {PAR_UI32,'F',{ 433000000UL } },  // base frequency
    {PAR_UI8,'M',{ 0xA0 } }, // modulation MOD_2FSK
    {PAR_UI32,'R',{ 12500UL }}, // datarate
    {PAR_UI32,'W',{ 50000UL }}, // bandwidth
    {PAR_UI32,'D',{ 12500UL }}, // freq_deviation
    {PAR_UI32,'S',{ 50000UL }}, // channel space
    {PAR_I32,'P',{ 16L }}, // power
    {PAR_UI8,'T',{ 1 }}, // transmit/rec
    {PAR_UI8,'L',{ 1 }}, // use LDO/bypass LDO
    {PAR_I32,'C',{ 21 }}, // channel
    {PAR_UI32,'E',{ 64 }}, // preamble length
    {PAR_UI32,'N',{ 0x00000301 }}, // id
    {PAR_UI32,'I',{ 30 }}, // interval in seconds
    {PAR_UI8,'X',{ 3 }}, // repeater
    {PAR_UI8,'Y',{ 0x05 }}, // JP4 mode, 0-inactive, 1 - change status, 2 - if alarm - non-stop, 0x04 bit: if set JP4 1 - norm, 0 - alarm
    {PAR_UI8,'Z',{ 0x06 }}, // JP5 mode, 0-inactive, 1 - change status, 2 - if alarm - non-stop, 0x04 bit: if set JP5 1 - norm, 0 - alarm
    {0,'\x00',{0}}
}; 

char t[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

char c_buf[BUF_LEN], val_buf[BUF_LEN];
uint8_t c_len;
uint8_t hex=0;
char prompt[] = {"\r\n> "};
char err[] = {"\r\nError\r\n> "};
char ex[] = {"\r\nExit\r\n"};
char commands[] = {'S', 'L', 'D'};
char ver[]={"=== S2-LP shell v 1.0.4 ===\r\n"};

void send_chars(char* x) {
    uint8_t i=0;
    while(x[i]!=0) EUSART1_Write(x[i++]);
    while (!EUSART1_is_tx_done());
}

void empty_RXbuffer() {
    while (EUSART1_is_rx_ready()) EUSART1_Read();
}

uint8_t stringToUInt32(char* str, uint32_t* val) //it is a function made to convert the string value to integer value.
{
    uint8_t i = 0;
    uint32_t sum = 0;
    if(str[0]=='0' && (str[1]=='x' || str[1]=='X'))
    {
        i+=2;
        while(str[i] != 0)
        {
           if (str[i] >= 0x30 && str[i] <= 0x39) sum=sum*16+(str[i]-0x30);
           else if(str[i] >= 0x41 && str[i] <= 0x46) sum=sum*16+(str[i]-0x41+10);
           else if(str[i] >= 0x61 && str[i] <= 0x66) sum=sum*16+(str[i]-0x41+10);
           else return 1;
           i++;
        }
    }
    else
    {
        while (str[i] != '\0') //string not equals to null
        {

            if (str[i] < 48 || str[i] > 57) return 1; // ascii value of numbers are between 48 and 57.
            else {
                sum = sum * 10 + (str[i] - 48);
                i++;
            }
        }
    }
    *val = sum;
    return 0;
}

uint8_t stringToUInt8(char* str, uint8_t* val) //it is a function made to convert the string value to integer value.
{
    int8_t i = -1;
    uint8_t sum = 0;
    if(str[0]=='0' && (str[1]=='x' || str[1]=='X'))
    {
        i+=2;
        while(str[++i] != 0)
        {
           if(i>=4) return 1;
           if (str[i] >= 0x30 && str[i] <= 0x39) sum=sum*16+(str[i]-0x30);
           else if(str[i] >= 0x41 && str[i] <= 0x46) sum=sum*16+(str[i]-0x41+10);
           else if(str[i] >= 0x61 && str[i] <= 0x66) sum=sum*16+(str[i]-0x41+10);
           else return 1;
        }
    }
    else
    {
        while (str[++i] != 0);
        if (i > 3) return 1;
        if (i == 3) {
            if (str[0] > 0x32) return 1;
            if (str[0] == 0x32) {
                if (str[1] > 0x35) return 1;
                if (str[0]==0x32 && str[1] == 0x35 && str[2] > 0x35) return 1;
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
    if(str[i]=='0' && (str[i+1]=='x' || str[i+1]=='X'))
    {
        i+=2;
        while(str[i] != 0)
        {
           if((i-sign)>=10) return 1;
           if (str[i] >= 0x30 && str[i] <= 0x39) sum=sum*16+(str[i]-0x30);
           else if(str[i] >= 0x41 && str[i] <= 0x46) sum=sum*16+(str[i]-0x41+10);
           else if(str[i] >= 0x61 && str[i] <= 0x66) sum=sum*16+(str[i]-0x41+10);
           else return 1;
           i++;
        }
    }
    else
    {
        while (str[i] != '\0') //string not equals to null
        {

            if (str[i] < 48 || str[i] > 57) return 1; // ascii value of numbers are between 48 and 57.
            else {
                sum = sum * 10 + (str[i] - 48);
                i++;
            }
        }
    }
    if (sign) *val = -sum;
    else *val = sum;
    return 0;
}

void _print_par(_par* par)
{
    if(par->type==PAR_UI32)
    {
        if(hex) ui32tox(par->u.ui32par, val_buf);
        else ui32toa(par->u.ui32par, val_buf);
    }
    if(par->type==PAR_I32)
    {
        if(hex) i32tox(par->u.i32par, val_buf);
        else i32toa(par->u.i32par, val_buf);
    }
    if(par->type==PAR_UI8)
    {
        if(hex) ui8tox(par->u.ui8par, val_buf);
        else ui8toa(par->u.ui8par, val_buf);
    }
    EUSART1_Write(par->c);
    EUSART1_Write('=');
    uint8_t i = 0;
    while (val_buf[i]) {
        EUSART1_Write(val_buf[i++]);
        while (!EUSART1_is_tx_done());
    }
    EUSART1_Write('\r');
    EUSART1_Write('\n');
    while (!EUSART1_is_tx_done());
}

void print_par(char p)
{
    _par* __pars=_pars;
    while(__pars->type)
    {
        if(__pars->c==p)
        {
             _print_par(__pars);
             return;
        }
        __pars++;
    }
}

void print_pars()
{
    _par* __pars=_pars;
    while(__pars->type)
    {
         _print_par(__pars);
        __pars++;
    }
}

uint8_t set_par(char par, char* val_buf)
{
    _par* __pars=_pars;
    while(__pars->type)
    {
        if(__pars->c==par)
        {
            if(__pars->type==PAR_UI32) { if (stringToUInt32(val_buf, &(__pars->u.ui32par))) return 1; };
            if(__pars->type==PAR_I32) { if (stringToInt32(val_buf, &(__pars->u.i32par))) return 1; };
            if(__pars->type==PAR_UI8) { if (stringToUInt8(val_buf, &(__pars->u.ui8par))) return 1; };
            return 0;
        };
        __pars++;
    }
    return 1;
}

uint8_t set_s(char p,void* s)
{
    _par* __pars=_pars;
    while(__pars->type)
    {
        if(__pars->c==p)
        {
            if(__pars->type==PAR_UI32) *((uint32_t*)s)=__pars->u.ui32par;
            if(__pars->type==PAR_I32)  *((int32_t*)s)=__pars->u.i32par;
            if(__pars->type==PAR_UI8)  *((uint8_t*)s)=__pars->u.ui8par;
            return 0;
        };
        __pars++;
    }
    return 1;
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

char* ui8tox(uint8_t i, char* b)
{
    char* p = b;
    *p++='0';
    *p++='x';
    *p++=t[i>>4];
    *p++=t[i&0x0f];
    *p=0;
    return b;
}

char* i32tox(int32_t i, char* b)
{
    return ui32tox((uint32_t)i,b);
}


char* ui32tox(uint32_t i, char* b)
{
    uint8_t* ch;
    ch=((uint8_t*)(&i));
    char* p = b;
    *p++='0';
    *p++='x';
    *p++=t[ch[3]>>4];
    *p++=t[ch[3]&0x0F];
    *p++=t[ch[2]>>4];
    *p++=t[ch[2]&0x0F];
    *p++=t[ch[1]>>4];
    *p++=t[ch[1]&0x0F];
    *p++=t[ch[0]>>4];
    *p++=t[ch[0]&0x0F];
    *p=0;
    return b;
}


uint8_t proceed() {
    uint8_t i = 0, par, val, cmd;
    //    printf("proceed %s\r\n",c_buf);
    c_buf[c_len] = 0;
    cmd = c_buf[i++];
    if(cmd==0) return 1;
    if(c_buf[1]=='X')
    {
        hex=1;
        i++;
    }
    else hex=0;
    if (cmd == 'Q' && c_buf[i] == 0) {
        send_exit();
        return 0;
    }
    if (cmd == 'L' && c_buf[i] == 0) {
        print_pars();
        return 1;
    }
    while (c_buf[i] == ' ' || c_buf[i] == '\t') i++;
    par = c_buf[i];
    uint8_t ip = 0, ip0 = 0xff;
    do {
        if (_pars[ip].c == par) {
            ip0 = ip;
            break;
        }
    } while (_pars[++ip].type);
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
    send_chars(ver);
    send_prompt();
    while (1) {
        if (!start) {
            if (TestTimer3()) {
                send_exit();
                return;
            }
        }
        if (EUSART1_is_rx_ready()) {
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
