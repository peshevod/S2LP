#include "mcc_generated_files/mcc.h"
#include "timers.h"


uint16_t c0_3,c0_5;

void delay_us(uint16_t d)
{
    TMR0_WriteTimer(0);
    while(TMR0_ReadTimer()<d);
    return;
}

void delay_ms(uint16_t d)
{
    TMR1_WriteTimer(0);
    uint16_t c0=d<<2;
    while(TMR1_ReadTimer()<c0);
    return;
}

void SetTimer3(uint16_t ms)
{
    TMR3_WriteTimer(0);
    c0_3=ms<<2;
}

uint8_t TestTimer3()
{
    if(TMR3_ReadTimer()>=c0_3) return 1;
    else return 0;
}

void SetTimer5(uint16_t ms)
{
    TMR5_WriteTimer(0);
    c0_5=ms<<2;
}

uint8_t TestTimer5()
{
    if(TMR5_ReadTimer()>=c0_5) return 1;
    else return 0;
}

void timers_init()
{
  uint8_t freq=OSCFRQbits.HFFRQ;
  if(freq<=3) T0CON1bits.CKPS=freq;
  else T0CON1bits.CKPS=freq-1;
  TMR0_StartTimer();
  TMR1_StartTimer();
  TMR3_StartTimer();
  TMR5_StartTimer();
}

