#include "MCU_Interface.h"
#include "mcc_generated_files/mcc.h"
//#include "main.h"
#include "timers.h"

#define MY_BUFFER_SIZE 32

uint8_t     myWriteBuffer[MY_BUFFER_SIZE];
uint8_t     myReadBuffer[MY_BUFFER_SIZE];
uint8_t state0,state1,reg;


void S2LPSpiInit(void)
{
    SPI1_Initialize();
}

void SdkSpiDeinit()
{
    
}

void S2LPEnterShutdown(void)
{
    SDN_SetHigh();
    delay_ms(100);
}

void S2LPExitShutdown(void)
{
    SDN_SetLow();
    delay_ms(10);
}


StatusBytes S2LPSpiWriteRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t* pcBuffer)
{
    myWriteBuffer[0]=OP_WRITE;
    myWriteBuffer[1]=cRegAddress;
    CSN_SetLow();
//        delay_us(2);
    myReadBuffer[0] = SPI1_ExchangeByte(myWriteBuffer[0]);
    myReadBuffer[1] = SPI1_ExchangeByte(myWriteBuffer[1]);
    for(uint8_t j=0;j<cNbBytes;j++)
    {
        myReadBuffer[j+2] = SPI1_ExchangeByte(pcBuffer[j]);
    }
//      delay_us(2);
    CSN_SetHigh();
/*    p('w',myWriteBuffer[0],myReadBuffer[0]);
    p('w',myWriteBuffer[1],myReadBuffer[1]);
    for(uint8_t j=0;j<cNbBytes;j++)
    {
        printf("%d",j);
        p(' ',pcBuffer[j],myReadBuffer[j+2]);
    }*/
    return STATUS0;
}


StatusBytes S2LPSpiReadRegisters(uint8_t cRegAddress, uint8_t cNbBytes, uint8_t* pcBuffer)
{
    myWriteBuffer[0]=OP_READ;
    myWriteBuffer[1]=cRegAddress;
    for(uint8_t j=0;j<cNbBytes;j++) myWriteBuffer[j+2]=0;
    CSN_SetLow();
//        for(uint8_t j=0;j<cNbBytes;j++)delay_us(2);
    myReadBuffer[0] = SPI1_ExchangeByte(myWriteBuffer[0]);
    myReadBuffer[1] = SPI1_ExchangeByte(myWriteBuffer[1]);
    for(uint8_t j=0;j<cNbBytes;j++)
    {
        pcBuffer[j] = SPI1_ExchangeByte(myWriteBuffer[j+2]);
    }
//      delay_us(2);
    CSN_SetHigh();
/*    p('r',myWriteBuffer[0],myReadBuffer[0]);
    p('r',myWriteBuffer[1],myReadBuffer[1]);
    for(uint8_t j=0;j<cNbBytes;j++)
    {
        printf("%d",j);
        p(' ',myWriteBuffer[j+2],pcBuffer[j]);
    }*/
   return STATUS0;
}

StatusBytes S2LPSpiCommandStrobes(uint8_t cCommandCode)
{
    myWriteBuffer[0]=OP_COMMAND;
    myWriteBuffer[1]=cCommandCode;
    CSN_SetLow();
//        delay_us(2);
    myReadBuffer[0] = SPI1_ExchangeByte(myWriteBuffer[0]);
    myReadBuffer[1] = SPI1_ExchangeByte(myWriteBuffer[1]);
//      delay_us(2);
    CSN_SetHigh();
/*    p('c',myWriteBuffer[0],myReadBuffer[0]);
    p('c',myWriteBuffer[1],myReadBuffer[1]);*/
    return STATUS0;
    
}

StatusBytes S2LPSpiWriteFifo(uint8_t cNbBytes, uint8_t* pcBuffer)
{
    myWriteBuffer[0]=OP_WRITE;
    myWriteBuffer[1]=0xFF;
    CSN_SetLow();
//        delay_us(2);
    myReadBuffer[0] = SPI1_ExchangeByte(myWriteBuffer[0]);
    myReadBuffer[1] = SPI1_ExchangeByte(myWriteBuffer[1]);
    for(uint8_t j=0;j<cNbBytes;j++)
    {
        myReadBuffer[2] = SPI1_ExchangeByte(pcBuffer[j]);
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
    myReadBuffer[0] = SPI1_ExchangeByte(myWriteBuffer[0]);
    myReadBuffer[1] = SPI1_ExchangeByte(myWriteBuffer[1]);
    for(uint8_t j=0;j<cNbBytes;j++)
    {
        pcBuffer[j] = SPI1_ExchangeByte(myWriteBuffer[2]);
    }
//      delay_us(2);
    CSN_SetHigh();
    return STATUS0;
}

void write_reg(uint8_t ad,uint8_t value)
{
    myWriteBuffer[0]=OP_WRITE;
    myWriteBuffer[1]=ad;
    myWriteBuffer[2]=value;
    CSN_SetLow();
    delay_us(2);
    for(uint8_t i=0;i<3;i++)
    {
        myReadBuffer[i] = SPI1_ExchangeByte(myWriteBuffer[i]);
    }
    CSN_SetHigh();
    state0=myReadBuffer[0];
    state1=myReadBuffer[1];
    delay_us(20);
}

/*uint8_t read_reg(uint8_t ad)
{
    myWriteBuffer[0]=OP_READ;
    myWriteBuffer[1]=ad;
    myWriteBuffer[2]=0;
    CSN_SetLow();
//    delay_us(2);
    for(uint8_t i=0;i<3;i++)
    {
        myReadBuffer[i] = SPI1_ExchangeByte(myWriteBuffer[i]);
    }
    CSN_SetHigh();
    state0=myReadBuffer[0];
    state1=myReadBuffer[1];
    reg=myReadBuffer[2];
//    delay_us(20);
    return myReadBuffer[2];
}*/

uint8_t read_reg(uint8_t ad)
{
    myWriteBuffer[0]=OP_READ;
    myWriteBuffer[1]=ad;
    myWriteBuffer[2]=0;
    CSN_SetLow();
//    delay_us(2);
    for(uint8_t i=0;i<3;i++)
    {
        myReadBuffer[i] = SPI1_ExchangeByte(myWriteBuffer[i]);
    }
    CSN_SetHigh();
//    delay_us(20);
    return myReadBuffer[2];
}


void send_command(char cmd)
{
    myWriteBuffer[0]=OP_COMMAND;
    myWriteBuffer[1]=cmd;
    CSN_SetLow();
    delay_us(2);
    for(uint8_t i=0;i<2;i++)
    {
        myReadBuffer[i] = SPI1_ExchangeByte(myWriteBuffer[i]);
    }
    CSN_SetHigh();
    state0=myReadBuffer[0];
    state1=myReadBuffer[1];
    delay_us(20);
}

