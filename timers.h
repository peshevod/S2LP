/* 
 * File:   timers.h
 * Author: ilya_000
 *
 * Created on 10 июля 2019 г., 19:25
 */

#ifndef TIMERS_H
#define	TIMERS_H

#ifdef	__cplusplus
extern "C" {
#endif

void delay_us(uint16_t d);
void delay_ms(uint16_t d);
void SetTimer3(uint16_t ms);
uint8_t TestTimer3();
void SetTimer5(uint16_t ms);
uint8_t TestTimer5();
void timers_init();


#ifdef	__cplusplus
}
#endif

#endif	/* TIMERS_H */

