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
    
    
void start_x_shell(void);
void send_chars(char* x);
uint8_t stringToUInt32(char* str, uint32_t* val);
uint8_t stringToUInt8(char* str, uint8_t* val);
uint8_t stringToInt32(char* str, int32_t* val);
char* i32toa(int32_t i, char* b);
char* ui32toa(uint32_t i, char* b);
char* ui8toa(uint8_t i, char* b);
char* ui8tox(uint8_t i, char* b);
char* ui32tox(uint8_t i, char* b);

#ifdef	__cplusplus
}
#endif

#endif	/* SHELL_H */

