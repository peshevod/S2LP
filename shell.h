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

#define S_WAIT_COMMAND      0
#define S_WAIT_PAR          1
#define S_DEFINE_COMMAND    2
    
void start_shell(void);

#ifdef	__cplusplus
}
#endif

#endif	/* SHELL_H */

