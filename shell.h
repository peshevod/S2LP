/* 
 * File:   shell.h
 * Author: ilya_000
 *
 * Created on July 13, 2019, 10:02 AM
 */

#ifndef SHELL_H
#define	SHELL_H

#ifdef	__cplusplus
extern "C" {
#endif

#define BUF_LEN 256

    
#define send_prompt()   {send_chars((char*)prompt);}  
#define send_error()   {send_chars((char*)err);}  
#define send_exit()   {send_chars((char*)ex);} 
    
typedef struct par
{
    uint8_t type;
    char c;
    union
    {
        uint32_t ui32par;
        int32_t i32par;
        uint8_t ui8par;
    } u;
} _par;

#define PAR_UI32    1
#define PAR_I32     2
#define PAR_UI8     3
    
    
void start_x_shell(void);
void send_chars(char* x);
uint8_t stringToUInt32(char* str, uint32_t* val);
uint8_t stringToUInt8(char* str, uint8_t* val);
uint8_t stringToInt32(char* str, int32_t* val);
char* i32toa(int32_t i, char* b);
char* ui32toa(uint32_t i, char* b);
char* ui8toa(uint8_t i, char* b);
char* ui8tox(uint8_t i, char* b);
char* ui32tox(uint32_t i, char* b);
char* i32tox(int32_t i, char* b);
void _print_par(_par* par);
void print_par(char p);
void print_pars();
uint8_t set_s(char p,void* s);
void get_uid(uint32_t* uid);
void set_uid(uint32_t uid);

#ifdef	__cplusplus
}
#endif

#endif	/* SHELL_H */

