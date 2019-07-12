#include "MCU_Interface.h"
#include "mcc_generated_files/mcc.h"

#define MY_BUFFER_SIZE 64

uint8_t     myWriteBuffer[MY_BUFFER_SIZE];
uint8_t     myReadBuffer[3];

void S2LPSpiInit(void)
{
    SPI1_Initialize();
}

void SdkSpiDeinit()
{
    
}

StatusBytes S2LPSpiWriteRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t* pcBuffer)
{
    myWriteBuffer[0]=OP_WRITE;
    myWriteBuffer[1]=cRegAddress;
    CSN_SetLow();
//        delay_us(2);
    myReadBuffer[0] = SPI1_Exchange8bit(myWriteBuffer[0]);
    myReadBuffer[1] = SPI1_Exchange8bit(myWriteBuffer[1]);
    for(uint8_t j=0;j<cNbBytes;j++)
    {
        myReadBuffer[2] = SPI1_Exchange8bit(pcBuffer[j]);
    }
//      delay_us(2);
    CSN_SetHigh();
    return STATUS0;
}


StatusBytes S2LPSpiReadRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t* pcBuffer)
{
    myWriteBuffer[0]=OP_READ;
    myWriteBuffer[1]=cRegAddress;
    myWriteBuffer[2]=0;
    CSN_SetLow();
//        delay_us(2);
    myReadBuffer[0] = SPI1_Exchange8bit(myWriteBuffer[0]);
    myReadBuffer[1] = SPI1_Exchange8bit(myWriteBuffer[1]);
    for(uint8_t j=0;j<cNbBytes;j++)
    {
        pcBuffer[j] = SPI1_Exchange8bit(myWriteBuffer[2]);
    }
//      delay_us(2);
    CSN_SetHigh();
    return STATUS0;
}

StatusBytes S2LPSpiCommandStrobes(uint8_t cCommandCode)
{
    myWriteBuffer[0]=OP_COMMAND;
    myWriteBuffer[1]=cCommandCode;
    CSN_SetLow();
//        delay_us(2);
    myReadBuffer[0] = SPI1_Exchange8bit(myWriteBuffer[0]);
    myReadBuffer[1] = SPI1_Exchange8bit(myWriteBuffer[1]);
//      delay_us(2);
    CSN_SetHigh();
    return STATUS0;
    
}

StatusBytes S2LPSpiWriteFifo(uint8_t cNbBytes, uint8_t* pcBuffer)
{
    myWriteBuffer[0]=OP_WRITE;
    myWriteBuffer[1]=0xFF;
    CSN_SetLow();
//        delay_us(2);
    myReadBuffer[0] = SPI1_Exchange8bit(myWriteBuffer[0]);
    myReadBuffer[1] = SPI1_Exchange8bit(myWriteBuffer[1]);
    for(uint8_t j=0;j<cNbBytes;j++)
    {
        myReadBuffer[2] = SPI1_Exchange8bit(pcBuffer[j]);
    }
//      delay_us(2);
    CSN_SetHigh();
    return STATUS0;
}


StatusBytes S2LPSpiReadFifo(uint8_t cNbBytes, uint8_t* pcBuffer)
{
    myWriteBuffer[0]=OP_READ;
    myWriteBuffer[1]=0xFF;
    myWriteBuffer[2]=0;
    CSN_SetLow();
//        delay_us(2);
    myReadBuffer[0] = SPI1_Exchange8bit(myWriteBuffer[0]);
    myReadBuffer[1] = SPI1_Exchange8bit(myWriteBuffer[1]);
    for(uint8_t j=0;j<cNbBytes;j++)
    {
        pcBuffer[j] = SPI1_Exchange8bit(myWriteBuffer[2]);
    }
//      delay_us(2);
    CSN_SetHigh();
    return STATUS0;
}