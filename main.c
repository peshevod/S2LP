/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.76
        Device            :  PIC16LF18446
        Driver Version    :  2.00
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#include "mcc_generated_files/mcc.h"
#include "radio.h"
#include "S2LP_Config.h"
#include "timers.h"
#include "shell.h"

#define MODE_TX 0
#define MODE_RX 1

#define MAX_REC_SIZE 128

#define ALARM_JP4   0x01
#define ALARM_JP5   0x02
#define CLEAR_JP4   0xFE
#define CLEAR_JP5   0xFD

typedef enum 
{
    SLEEP = 0,
    COUNTER = 1,
    SEND = 2
} SLEEP_STATE;

/*
                         Main application
 */

uint8_t vectcTxBuff[20]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

uint8_t vectcRxBuff[MAX_REC_SIZE];

S2LPIrqs xIrqStatus;
volatile uint8_t irqf;
uint8_t mode,mode0,mode1,mode2;
extern volatile S2LPStatus g_xStatus;
extern volatile uint8_t inco1;
char pb[128];
uint32_t counter;
uint32_t rest;
uint8_t repeater,cur_repeater;
uint32_t next;
uint8_t packetlen;
uint8_t jp4_mode,jp5_mode;


void EXTI_Callback_INT(void)
{
    irqf++;
}

void EXTI_Callback_JP4(void)
{
    NOP();
    if(!(jp4_mode&0x03)) return;
    vectcTxBuff[9]|=ALARM_JP4;
}

void EXTI_Callback_JP5(void)
{
    NOP();
    if(!(jp5_mode&0x03)) return;
    vectcTxBuff[9]|=ALARM_JP5;
}

void pmd_set(SLEEP_STATE ss)
{

    if(ss==SEND)
    {
//        PMD0bits.NVMMD=0;
//        PMD0bits.SYSCMD=0;
        PMD0bits.CLKRMD=1;
        PMD0bits.IOCMD=0;
        
        PMD1bits.TMR0MD=0;
        PMD1bits.TMR1MD=0;
        PMD1bits.TMR3MD=0;
        PMD1bits.TMR5MD=0;

        PMD2bits.NCO1MD=1;

        PMD6bits.MSSP1MD=0;
        PMD6bits.U1MD=0;
        
        TMR0_Initialize();
        TMR1_Initialize();
        TMR3_Initialize();
        TMR5_Initialize();
        SPI1_Initialize();
        EUSART1_Initialize();
//        IOCAFbits.IOCAF2 = 0;
//        IOCANbits.IOCAN2 = 1;
//        IOCAPbits.IOCAP2 = 0;
//        PIE0bits.IOCIE = 1; 
//        PIR0bits.IOCIF = 0;
    }
    if(ss==SLEEP)
    {
//        PMD0bits.NVMMD=0;
//        PMD0bits.SYSCMD=0;
        PMD0bits.CLKRMD=0;
        PMD0bits.IOCMD=0;
        
        PMD1bits.TMR0MD=1;
        PMD1bits.TMR1MD=1;
        PMD1bits.TMR3MD=1;
        PMD1bits.TMR5MD=1;

        PMD2bits.NCO1MD=0;

        PMD6bits.MSSP1MD=1;
        PMD6bits.U1MD=1;
        
        CLKREF_Initialize();
        NCO1CON = 0x00;
        // CKS CLKR; PWS 1_clk; 
        NCO1CLK = 0x06;
        NCO1INCU = 0x00;
        NCO1INCH = 0x00;
        NCO1INCL = 0x01;
        NCO1ACCU = (rest&0x00FF0000)>>16;
        NCO1ACCH = (rest&0x0000FF00)>>8;
        NCO1ACCL = (rest&0x000000FF);
        NCO1CONbits.EN = 1;
        PIR7bits.NCO1IF = 0;
        PIE7bits.NCO1IE = 1;
    }
    if(ss==COUNTER)
    {
//        PMD0bits.NVMMD=1;
//        PMD0bits.SYSCMD=1;
        PMD0bits.CLKRMD=1;
//        PMD0bits.IOCMD=1;
        
        PMD1bits.TMR0MD=1;
        PMD1bits.TMR1MD=1;
        PMD1bits.TMR3MD=1;
        PMD1bits.TMR5MD=1;

        PMD2bits.NCO1MD=1;

        PMD6bits.MSSP1MD=1;
        PMD6bits.U1MD=1;
    }
}

void pmd_off(void)
{
    PMD0bits.FVRMD=1;

    PMD1bits.TMR2MD=1;
    PMD1bits.TMR4MD=1;
    PMD1bits.TMR6MD=1;
    
    PMD3bits.ADCMD=1;
    PMD3bits.CMP1MD=1;
    PMD3bits.CMP2MD=1;
    PMD3bits.DAC1MD=1;
    PMD3bits.ZCDMD=1;
    
    PMD4bits.CCP1MD=1;
    PMD4bits.CCP2MD=1;
    PMD4bits.CCP3MD=1;
    PMD4bits.CCP4MD=1;
    PMD4bits.PWM6MD=1;
    PMD4bits.PWM7MD=1;
    
    PMD5bits.CWG1MD=1;
    PMD5bits.CWG2MD=1;

    PMD6bits.MSSP2MD=1;
    
    
    PMD7bits.CLC1MD=1;
    PMD7bits.CLC2MD=1;
    PMD7bits.CLC3MD=1;
    PMD7bits.CLC4MD=1;
    PMD7bits.DSM1MD=1;
    PMD7bits.SMT1MD=1;
}

void init_pic(uint8_t shell)
{
    uint8_t tmp;
    timers_init();
    if(shell) start_x_shell();
    set_s('T',&tmp);
    if(tmp)
    {
        mode=MODE_TX;
        send_chars("Transmit mode\r\n");
    }
    else
    {
        mode=MODE_RX;
        send_chars("Receive mode\r\n");
    }
}

void to_sleep(void)
{
    uint32_t tmp32;
    uint64_t div;
    set_s('I',&tmp32);
    counter=(tmp32*1000)/4329559;
    div=(tmp32*1000)-(counter*4329559);
    if(div==0) counter--;
    rest=1048576-(div*24219)/100000;
    send_chars("Going to sleep rest=");
    send_chars(ui32tox(rest,pb));
    send_chars(" counter=");
    send_chars(ui32tox(counter,pb));
    send_chars("\r\n");
    pmd_set(SLEEP);
//    pmd_set(SEND);
//    VREGCONbits.VREGPM=1;
    while(1)
    {
        CPUDOZEbits.IDLEN=0;
        SLEEP();
        NOP();
        if(!vectcTxBuff[9])
        {
            pmd_set(COUNTER);
            if(counter-->0)
            {
                rest=0;
                pmd_set(SLEEP);
                continue;
            }
        }
        pmd_set(SEND);
        break;
    }
    init_pic(0);
    send_chars("Coming from sleep\r\n");
}


void main(void)
{
    // initialize the device
    SYSTEM_Initialize();

    // When using interrupts, you need to set the Global and Peripheral Interrupt Enable bits
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Enable the Peripheral Interrupts
    INTERRUPT_PeripheralInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
    send_chars("Begin\r\n");
//    alarm4=0;
//    alarm5=0;
    pmd_off();
    pmd_set(SEND);
    init_pic(1);
    IOCAF2_SetInterruptHandler(EXTI_Callback_INT);
    mode0=0;
    mode1=0;
    mode2=0;
    IOCCPbits.IOCCP5=1;
    IOCCNbits.IOCCN5=1;
    set_s('Y',&jp4_mode);
    if((jp4_mode&0x03)!=0)
    {
        mode0&=CLEAR_JP4;
        if((jp4_mode&0x03)==1)
        {
            mode1|=ALARM_JP4;
            mode2&=CLEAR_JP4;
        }
        else
        {
            if(jp4_mode&0x04) IOCCPbits.IOCCP5=0;
            else IOCCNbits.IOCCN5=0;
            mode2|=ALARM_JP4;
            mode1&=CLEAR_JP4;
        }
        IOCCF5_SetInterruptHandler(EXTI_Callback_JP4);
    }
    else
    {
        IOCCPbits.IOCCP5=0;
        IOCCNbits.IOCCN5=0;
        mode0|=ALARM_JP4;
        mode1&=CLEAR_JP4;
        mode2&=CLEAR_JP4;
    }
    set_s('Z',&jp5_mode);
    IOCCNbits.IOCCN4=1;
    IOCCPbits.IOCCP4=1;
    if((jp5_mode&0x03)!=0)
    {
        mode0&=CLEAR_JP5;
        if((jp5_mode&0x03)==1)
        {
            mode1|=ALARM_JP5;
            mode2&=CLEAR_JP5;
        }
        else
        {
            if(jp5_mode&0x04) IOCCPbits.IOCCP4=0;
            else IOCCNbits.IOCCN4=0;
            mode2|=ALARM_JP5;
            mode1&=CLEAR_JP5;
        }
        IOCCF4_SetInterruptHandler(EXTI_Callback_JP5);
    }
    else
    {
        IOCCNbits.IOCCN4=0;
        IOCCPbits.IOCCP4=0;
        mode0|=ALARM_JP5;
        mode1&=CLEAR_JP5;
        mode2&=CLEAR_JP5;
    }
    packetlen=12;

    if(mode!=MODE_RX)
    {
        radio_tx_init(packetlen);
        vectcTxBuff[0]=0;
        vectcTxBuff[1]=0;
        vectcTxBuff[2]=0xFF;
        set_s('X',&repeater);
        vectcTxBuff[3]=repeater;
        set_s('N',&(vectcTxBuff[4]));
        next=((uint32_t*)vectcTxBuff)[1];
        vectcTxBuff[9]=0;
        vectcTxBuff[10]=jp4_mode;
        vectcTxBuff[11]=jp5_mode;
        while (1)
        {
            vectcTxBuff[8]=0;
            if(JP4_GetValue()^((jp4_mode&0x04)>>2))
            {
                vectcTxBuff[8]|=ALARM_JP4;
                vectcTxBuff[9]|=ALARM_JP4;
            }
            if(JP5_GetValue()^((jp5_mode&0x04)>>2))
            {
                vectcTxBuff[8]|=ALARM_JP5;
                vectcTxBuff[9]|=ALARM_JP5;
            }
            send_chars("A data to transmit: [");
            for(uint8_t i=0 ; i<packetlen ; i++)
            {
                send_chars(ui8toa(vectcTxBuff[i],pb));
                send_chars(" ");
            }
            send_chars("]\n\r");
     
             /* fit the TX FIFO */
             S2LPCmdStrobeFlushTxFifo();
             S2LPSpiWriteFifo(packetlen, vectcTxBuff);

             /* send the TX command */
             S2LPCmdStrobeTx();

             /* wait for TX done */
            while(1)
            {
                if(irqf)
                {
                    S2LPGpioIrqGetStatus(&xIrqStatus);
                    if(((uint32_t*)(&xIrqStatus))[0] & IRQ_TX_DATA_SENT)
                    {
                        send_chars("Data sent\r\n");
                        /* sleep between transmissions */
                        if(--vectcTxBuff[3] || (vectcTxBuff[9]&mode2) )
                        {
                            next=1664525*next+1013904223;
                            delay_ms((next&0xFFFF0000)>>18);
                        }
                        else
                        {
                            SDN_SetHigh();
                            vectcTxBuff[9]&=mode2;
                            to_sleep();
                            SDN_SetLow();
                            radio_tx_init(packetlen);
                            vectcTxBuff[0]++;
                            if (vectcTxBuff[0]==0) vectcTxBuff[1]++;
                            vectcTxBuff[2]=0xFF;
                            vectcTxBuff[3]=repeater;
                        }
                    }
                    irqf=0;
                    break;
                }
                else
                {
                    S2LPRefreshStatus();
                    send_chars("Refresh Status ");
                    send_chars(ui8tox(g_xStatus.MC_STATE,pb));
                    send_chars("\r\n");
                    if(g_xStatus.MC_STATE!=0x5c)
                    {
                        if(irqf) continue;
                        radio_tx_init(packetlen);
                        vectcTxBuff[2]=g_xStatus.MC_STATE;
                        break;
                    }
                }
            }
        }
    }
    else
    {
        radio_rx_init(packetlen);
        while (1)
        {
            if(!irqf)
            {
                S2LPCmdStrobeRx();
                S2LPRefreshStatus();
                if(g_xStatus.MC_STATE!=0x30)
                {
/*                    send_chars("After CMD_RX Refresh Status ");
                    send_chars(ui8tox(g_xStatus.MC_STATE,pb));
                    send_chars("\r\n");*/
                    if(g_xStatus.MC_STATE!=0x0) radio_rx_init(packetlen);
                    continue;
                }
            }
            if(irqf)
            {
                
                S2LPGpioIrqGetStatus(&xIrqStatus);
/*                send_chars("INT!! irq=");
                send_chars(ui32tox(*((uint32_t*)(&xIrqStatus)),pb));
                send_chars("\r\n");*/
                if(xIrqStatus.RX_DATA_READY)
                {
                    //Get the RX FIFO size 
                    uint8_t cRxData = S2LPFifoReadNumberBytesRxFifo();

                    //Read the RX FIFO
                    S2LPSpiReadFifo(cRxData, vectcRxBuff);

//                    S2LPCmdStrobeRx();
                    //Flush the RX FIFO 
                    S2LPCmdStrobeFlushRxFifo();      
                    send_chars("REC:");
                    send_chars(ui32tox(((uint32_t*)vectcRxBuff)[0],pb));
                    send_chars(" ");
                    send_chars(ui32tox(((uint32_t*)vectcRxBuff)[1],pb));
                    send_chars(" ");
                    send_chars(ui32tox(((uint32_t*)vectcRxBuff)[2],pb));
                    send_chars(" ");
                    send_chars(i32toa(S2LPRadioGetRssidBm(),pb));
                    send_chars("\r\n");
                    //print the received data 
/*                    send_chars("B data received: [");
                    for(uint8_t i=0 ; i<cRxData ; i++)
                    {
                        send_chars(ui8toa(vectcRxBuff[i],pb));
                        send_chars(" ");
                    }
                    send_chars("]\n\r");*/
                }
                else if(xIrqStatus.RX_DATA_DISC)
                {
                    send_chars("DIS\r\n");
                    //RX command - to ensure the device will be ready for the next reception 
//                    S2LPCmdStrobeRx();
                }
                else
                {
                    send_chars("MES: Unknokwn interrupt Refresh Status ");
                    send_chars(ui8tox(g_xStatus.MC_STATE,pb));
                    send_chars("\r\n");
//                    S2LPCmdStrobeRx();
                }
                irqf=0;
            }
            else
            {
                if (EUSART1_is_rx_ready() && EUSART1_Read()=='R') RESET();
                S2LPRefreshStatus();
                if(g_xStatus.MC_STATE!=0x30)
                {
/*                    send_chars("MC_STATE!=0x30 Refresh Status ");
                    send_chars(ui8tox(g_xStatus.MC_STATE,pb));
                    send_chars(" irqf=");
                    send_chars(ui8toa(irqf,pb));
                    send_chars("\r\n");*/
                    if(!irqf) radio_rx_init(packetlen);
//                    S2LPCmdStrobeRx();
               }
            }
        }
    }
}
/**
 End of File
*/