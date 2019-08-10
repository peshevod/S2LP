/* 
 * File:   MCU_Interface.h
 * Author: ilya_000
 *
 * Created on May 23, 2019, 8:24 PM
 */

#ifndef MCU_INTERFACE_H
#define	MCU_INTERFACE_H

#include "S2LP_Config.h"
//#include "MCU_Interface_template.h"


#ifdef	__cplusplus
extern "C" {
#endif

#define OP_WRITE        0x00
#define OP_READ         0x01
#define OP_COMMAND      0x80
    
#define STATUS0 *((StatusBytes*)(myReadBuffer))

typedef S2LPStatus StatusBytes;

void S2LPSpiInit(void);
void S2LPSpiDeinit(void);
void S2LPEnterShutdown(void);
void S2LPExitShutdown(void);
StatusBytes S2LPSpiWriteRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t* pcBuffer);
StatusBytes S2LPSpiReadRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t* pcBuffer);
StatusBytes S2LPSpiCommandStrobes(uint8_t cCommandCode);
StatusBytes S2LPSpiWriteFifo(uint8_t cNbBytes, uint8_t* pcBuffer);
StatusBytes S2LPSpiReadFifo(uint8_t cNbBytes, uint8_t* pcBuffer);
void write_reg(uint8_t ad,uint8_t value);
uint8_t read_reg(uint8_t ad);
void send_command(char cmd);

#ifdef	__cplusplus
}
#endif

#endif	/* MCU_INTERFACE_H */

