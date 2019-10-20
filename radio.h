/* 
 * File:   radio.h
 * Author: ilya_000
 *
 * Created on July 4, 2019, 9:24 PM
 */

#ifndef RADIO_H
#define	RADIO_H

#ifdef	__cplusplus
extern "C" {
#endif

//#define USE_HIGH_BAND
#define USE_MIDDLE_BAND
 
 
/*  Radio configuration parameters  */
 
#ifdef USE_MIDDLE_BAND
#define BASE_FREQUENCY              433000000
#endif
 
#ifdef USE_HIGH_BAND
#define BASE_FREQUENCY              868000000
#endif
 
#define MODULATION_SELECT           MOD_2FSK
//#define MODULATION_SELECT           MOD_NO_MOD
#define DATARATE                    38400
#define FREQ_DEVIATION              20000
#define BANDWIDTH                   100000
 
#define POWER_DBM                   14/*.0*/
 
 
   
/*  Packet configuration parameters  */
#define PREAMBLE_LENGTH             PREAMBLE_BYTE(48)
#define SYNC_LENGTH                 SYNC_BYTE(4)
#define SYNC_WORD                   0x88888888
#define VARIABLE_LENGTH             S_ENABLE
#define EXTENDED_LENGTH_FIELD       S_DISABLE
#define CRC_MODE                    PKT_CRC_MODE_8BITS
#define EN_ADDRESS                  S_DISABLE
#define EN_FEC                      S_ENABLE
#define EN_WHITENING                S_ENABLE
 
 
/* Wake Up timer in us for LDC mode */
#define WAKEUP_TIMER                7000/*.0*/
#define FAST_RX_TIMER               500
 
    
#define PREAMBLE_BYTE(v)        (4*v)
#define SYNC_BYTE(v)            (8*v)
    
#define XTAL_FREQ               50000000

void radio_tx_init(uint8_t packetlen);
void radio_rx_init(uint8_t packetlen);
void radio_init(uint8_t packetlen);

#ifdef	__cplusplus
}
#endif

#endif	/* RADIO_H */

