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

/*
                         Main application
 */

uint8_t vectcTxBuff[20]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20};
volatile SFlagStatus xTxDoneFlag = S_RESET;
S2LPIrqs xIrqStatus;
volatile uint8_t irqf;
extern volatile S2LPStatus g_xStatus;

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
    printf("Begin\r\n");
    timers_init();
    
    start_x_shell();
    
    IOCAF2_SetInterruptHandler(EXTI_Callback_INT);
    while (1)
    {
        radio_init();
        irqf=0;
//ifdef USE_VCOM
     printf("A data to transmit: [");
     
     for(uint8_t i=0 ; i<20 ; i++)
       printf("%d ", vectcTxBuff[i]);
     printf("]\n\r");
//#endif
     
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
             printf("IRQ 0x%08lx\r\n",((uint32_t*)(&xIrqStatus))[0]);
             if(((uint32_t*)(&xIrqStatus))[0] & IRQ_TX_DATA_SENT)
             {
                 printf("Data sent\r\n");
             }
             irqf=0;
             break;
         }
         else
         {
            S2LPRefreshStatus();
            printf("Refresh Status 0x%02hhx\r",g_xStatus.MC_STATE);
            if(g_xStatus.MC_STATE==0x14)
            {
                S2LPEnterShutdown();
                S2LPExitShutdown();
                break;
            }
         }
     }
 
     /* pause between two transmissions */
     delay_ms(500);
     
    }
}
/**
 End of File
*/