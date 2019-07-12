#include "mcc_generated_files/mcc.h"
#include "timers.h"


uint16_t c0_3;
uint16_t c1_3;
uint8_t c2_3;
uint16_t c3_3;
uint16_t c0_5;
uint16_t c1_5;
uint8_t c2_5;
uint16_t c3_5;

void delay_us(uint16_t d)
{
    PIR0bits.TMR0IF=0;
    uint16_t c0=TMR0_ReadTimer();
    uint16_t c1=c0+d;
    if(c1<c0) while(!TMR0_HasOverflowOccured());
    PIR0bits.TMR0IF=0;
    while(TMR0_ReadTimer()<c1);
}

void delay_ms(uint16_t d)
{
    PIR4bits.TMR1IF=0;
    uint16_t c0=TMR1_ReadTimer();
    uint16_t c1=c0+(d<<2);
    if(c1<c0) while(!TMR1_HasOverflowOccured());
    PIR4bits.TMR1IF=0;
    while(TMR1_ReadTimer()<c1);
    return;
}

void SetTimer3(uint16_t ms)
{
    PIR4bits.TMR3IF=0;
    c0_3=TMR3_ReadTimer();
    c1_3=c0_3+(ms<<2);
    if(c1_3<c0_3) c2_3=1;
    else c2_3=0;
}

uint8_t TestTimer3()
{
    if(c2_3)
    {
        if(TMR3_HasOverflowOccured())
        {
            PIR4bits.TMR3IF=0;
            c2_3=0;
            c3_3=(0xFFFF-(c0_3-TMR3_ReadTimer())+1)>>2;
        }
        else c3_3=(TMR3_ReadTimer()-c0_3)>>2;
    }
    else
    {
        c3_3=(TMR3_ReadTimer()-c0_3)>>2;
        if(TMR3_ReadTimer()>=c1_3) return 1;
    }    
    return 0;    
}

void SetTimer5(uint16_t ms)
{
    PIR4bits.TMR5IF=0;
    c0_5=TMR5_ReadTimer();
    c1_5=c0_5+(ms<<2);
    if(c1_5<c0_5) c2_5=1;
    else c2_5=0;
}

uint8_t TestTimer5()
{
    if(c2_5)
    {
        if(TMR5_HasOverflowOccured())
        {
            PIR4bits.TMR5IF=0;
            c2_5=0;
            c3_5=(0xFFFF-(c0_5-TMR5_ReadTimer())+1)>>2;
        }
        else c3_5=(TMR5_ReadTimer()-c0_5)>>2;
    }
    else
    {
        c3_5=(TMR5_ReadTimer()-c0_5)>>2;
        if(TMR5_ReadTimer()>=c1_5) return 1;
    }    
//    printf("Timer5 waiting %d ms\r\n",c3_5);
    return 0;    
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

