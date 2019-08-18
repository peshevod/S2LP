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

/*
                         Main application
 */

uint8_t vectcTxBuff[20]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};

uint8_t vectcRxBuff[MAX_REC_SIZE];

S2LPIrqs xIrqStatus;
volatile uint8_t irqf;
uint8_t mode,bypass_ldo=0;
extern volatile S2LPStatus g_xStatus;
char pb[128];


void EXTI_Callback_INT(void)
{
    irqf++;
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
    timers_init();
    
    start_x_shell();
    if(JP3_GetValue())
    {
        bypass_ldo=0;
        send_chars("NO BYPASS_LDO\r\n");
    }
    else
    {
        bypass_ldo=1;
        send_chars("BYPASS_LDO\r\n");
    }
    
    if(JP2_GetValue())
    {
        mode=MODE_TX;
        send_chars("Transmit mode\r\n");
    }
    else
    {
        mode=MODE_RX;
        send_chars("Receive mode\r\n");
    }

    IOCAF2_SetInterruptHandler(EXTI_Callback_INT);

    if(mode!=MODE_RX)
    {
        radio_tx_init();
        while (1)
        {
            send_chars("A data to transmit: [");
            for(uint8_t i=0 ; i<20 ; i++)
            {
                send_chars(ui8toa(vectcTxBuff[i],pb));
                send_chars(" ");
            }
            send_chars("]\n\r");
     
             /* fit the TX FIFO */
             S2LPCmdStrobeFlushTxFifo();
             S2LPSpiWriteFifo(20, vectcTxBuff);

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
                        radio_tx_init();
                        break;
                    }
                }
            }
             /* pause between two transmissions */
             delay_ms(500);
        }
    }
    else
    {
        radio_rx_init();
        while (1)
        {
            if(!irqf)
            {
                S2LPCmdStrobeRx();
                S2LPRefreshStatus();
                if(g_xStatus.MC_STATE!=0x30)
                {
                    send_chars("After CMD_RX Refresh Status ");
                    send_chars(ui8tox(g_xStatus.MC_STATE,pb));
                    send_chars("\r\n");
                    if(g_xStatus.MC_STATE!=0x0) radio_rx_init();
                    continue;
                }
            }
            if(irqf)
            {
                
                S2LPGpioIrqGetStatus(&xIrqStatus);
                send_chars("INT!! irq=");
                send_chars(ui32tox(*((uint32_t*)(&xIrqStatus)),pb));
                send_chars("\r\n");
                if(xIrqStatus.RX_DATA_READY)
                {
                    //Get the RX FIFO size 
                    uint8_t cRxData = S2LPFifoReadNumberBytesRxFifo();

                    //Read the RX FIFO
                    S2LPSpiReadFifo(cRxData, vectcRxBuff);

//                    S2LPCmdStrobeRx();
                    //Flush the RX FIFO 
                    S2LPCmdStrobeFlushRxFifo();      

                    SBool xCorrect=S_TRUE;
                    for(uint8_t i=0 ; i<cRxData ; i++)
                        if(vectcRxBuff[i] != i+1)
                            xCorrect=S_FALSE;
                    if(xCorrect)
                    {
                        send_chars("DATA CORRECT, RSSI=");
                        send_chars(i32toa(S2LPRadioGetRssidBm(),pb));
                        send_chars(" dbm\r\n");
                    }
                    //print the received data 
                    send_chars("B data received: [");
                    for(uint8_t i=0 ; i<cRxData ; i++)
                    {
                        send_chars(ui8toa(vectcRxBuff[i],pb));
                        send_chars(" ");
                    }
                    send_chars("]\n\r");
                }
                else if(xIrqStatus.RX_DATA_DISC)
                {
                    send_chars("DATA DISCARDED\n\r");
                    //RX command - to ensure the device will be ready for the next reception 
//                    S2LPCmdStrobeRx();
                }
                else
                {
                    send_chars("Unknokwn interrupt Refresh Status ");
                    send_chars(ui8tox(g_xStatus.MC_STATE,pb));
                    send_chars("\r\n");
//                    S2LPCmdStrobeRx();
                }
                irqf=0;
            }
            else
            {
                S2LPRefreshStatus();
                if(g_xStatus.MC_STATE!=0x30)
                {
                    send_chars("MC_STATE!=0x30 Refresh Status ");
                    send_chars(ui8tox(g_xStatus.MC_STATE,pb));
                    send_chars(" irqf=");
                    send_chars(ui8toa(irqf,pb));
                    send_chars("\r\n");
                    if(!irqf) radio_rx_init();
//                    S2LPCmdStrobeRx();
               }
            }
        }
    }
}
/**
 End of File
*/