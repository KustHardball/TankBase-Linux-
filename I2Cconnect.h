#include <stdint.h>

void init_I2Cconnect();
uint16_t* get_voltage();
uint16_t* get_current();
void send_byte(uint8_t msg);
