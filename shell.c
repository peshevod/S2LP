#include "mcc_generated_files/eusart1.h"
#include "shell.h"
#include <stdio.h>
#include <ctype.h>

extern volatile uint8_t eusart1RxCount;
char c_buf[256];
uint8_t c_len;
char commands[][]={"SET","SHOW"};
uint8_t state=S_WAIT_COMMAND;


int8_t def_command(uint8_t len)
{
    for(uint8_t i=0;i<len;i++)
    {
        int c=toupper(c_buf[i]);
    }
        
}

void start_shell()
{
    char c;
    EUSART1_Initialize();
    c_len=0;
    state=S_WAIT_COMMAND;
    while(1)
    {
        if(eusart1RxCount!=0)
        {
            c=getch();
            putch(c);
            if(c==' '||c=='\n'||c=='\r')
            {
                if(state==S_WAIT_COMMAND)
                {
                    state=S_DEFINE_COMMAND;
                    def_command();
                    
            }
            c_buf[c_len++]=c;
            if(c)
        }
    }
}
