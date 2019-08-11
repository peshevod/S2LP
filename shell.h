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

    
#define send_prompt()   {send_chars(prompt,4);}  
#define send_error()   {send_chars(err,11);}  
#define send_exit()   {send_chars(ex,8);}  
    
    
void start_x_shell(void);
void send_chars(char* x, uint8_t len);

#ifdef	__cplusplus
}
#endif

#endif	/* SHELL_H */

