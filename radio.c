#include "S2LP_Config.h"
#include "radio.h"

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
   S2LP_GPIO_3,
   S2LP_GPIO_MODE_DIGITAL_OUTPUT_LP,
   S2LP_GPIO_DIG_OUT_IRQ
};
 
void radio_init()
{
    /* S2LP ON */
//   S2LPEnterShutdown();
//   S2LPExitShutdown();
    
    S2LPRadioSetXtalFrequency(XTAL_FREQ);

    /* S2LP IRQ config */
    S2LPGpioInit(&xGpioIRQ);
    
   /* S2LP Radio config */
   S2LPRadioInit(&xRadioInit);
    
   /* S2LP Radio set power */
   S2LPRadioSetMaxPALevel(S_DISABLE);
   S2LPRadioSetPALeveldBm(7,POWER_DBM);
   S2LPRadioSetPALevelMaxIndex(7);
   
   /* S2LP Packet config */
   S2LPPktBasicInit(&xBasicInit);
   
   /* S2LP IRQs enable */
   S2LPGpioIrqDeInit(NULL);
   S2LPGpioIrqConfig(TX_DATA_SENT , S_ENABLE);
   
   /* payload length config */
   S2LPPktBasicSetPayloadLength(20);
   
   /* IRQ registers blanking */
   S2LPGpioIrqClearStatus();
  
}


