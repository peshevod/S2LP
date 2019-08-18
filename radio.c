#include "S2LP_Config.h"
#include "radio.h"

extern uint8_t bypass_ldo;
extern volatile uint8_t irqf;

SRadioInit xRadioInit = {
   BASE_FREQUENCY,
   MODULATION_SELECT,
   DATARATE,
   FREQ_DEVIATION,
   BANDWIDTH
};
 
 
PktBasicInit xBasicInit={
   PREAMBLE_LENGTH,
   SYNC_LENGTH,
   SYNC_WORD,
   VARIABLE_LENGTH,
   EXTENDED_LENGTH_FIELD,
   CRC_MODE,
   EN_ADDRESS,
   EN_FEC,
   EN_WHITENING
};
 
 
SGpioInit xGpioIRQ={
   S2LP_GPIO_0,
   S2LP_GPIO_MODE_DIGITAL_OUTPUT_LP,
   S2LP_GPIO_DIG_OUT_IRQ
};
 

void radio_init()
{
    uint8_t tmp;
    /* S2LP ON */
    S2LPEnterShutdown();
    S2LPExitShutdown();
    
    S2LPRadioSetXtalFrequency(XTAL_FREQ);

    /* S2LP IRQ config */
    S2LPGpioInit(&xGpioIRQ);
    
    /* S2LP Radio config */
    S2LPRadioInit(&xRadioInit);
    
    /* S2LP Packet config */
    S2LPPktBasicInit(&xBasicInit);
   
    /* S2LP IRQs enable */
    S2LPGpioIrqDeInit(NULL);
   
    /* payload length config */
    S2LPPktBasicSetPayloadLength(20);
   
    /* IRQ registers blanking */
    S2LPGpioIrqClearStatus();
    
    if(bypass_ldo)
    {
        S2LPSpiReadRegisters(0x78, 1, &tmp);
        tmp|=0x04;
        S2LPSpiWriteRegisters(0x78, 1, &tmp);
    }
   

}

void radio_tx_init()
{
    uint8_t tmp;
    radio_init();
   
    /* S2LP Radio set power */
//   S2LPRadioSetMaxPALevel(S_DISABLE);
    S2LPRadioSetPALeveldBm(0,POWER_DBM);
    S2LPRadioSetPALevelMaxIndex(0);

    /* Maximum POWER*/
   
    S2LPSpiReadRegisters(PM_CONF0_ADDR, 1, &tmp);
    tmp|=SET_SMPS_LVL_REGMASK;
    S2LPSpiWriteRegisters(PM_CONF0_ADDR, 1, &tmp);
    S2LPRadioSetMaxPALevel(S_ENABLE);
   
    /* S2LP IRQs enable */
    S2LPGpioIrqConfig(IRQ_TX_DATA_SENT , S_ENABLE);
    
    /* IRQ registers blanking */
    S2LPGpioIrqClearStatus();
    irqf=0;
  
}

void radio_rx_init()
{
    radio_init();
    
    /* RX timeout config */
//    S2LPTimerSetRxTimerUs(700000);
    SET_INFINITE_RX_TIMEOUT();
    
    S2LPGpioIrqConfig(IRQ_RX_DATA_DISC,S_ENABLE);
    S2LPGpioIrqConfig(IRQ_RX_DATA_READY,S_ENABLE);

    /* IRQ registers blanking */
    S2LPGpioIrqClearStatus();
    irqf=0;
}


