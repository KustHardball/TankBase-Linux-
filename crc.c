
#include <stdint.h>
#include "var.h"



static uint16_t CRC16(uint16_t crc_calc, uint8_t value){
uint8_t i;
crc_calc = crc_calc ^ (uint16_t)value<<8;
for(i=0; i<8; i++) {
	if (crc_calc & 0x8000) crc_calc = (crc_calc << 1) ^ CRC_POLYNOME;
	else crc_calc = (crc_calc << 1);
	}
	return crc_calc;
}



uint16_t crc16_calc(uint8_t* buf, uint8_t lenght){
	uint16_t crc = 0x0000;
	uint16_t pack_crc;
for (uint8_t i=0;i<lenght;i++){
crc=CRC16(crc,buf[i]);
}
return crc;
}

