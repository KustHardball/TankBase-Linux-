#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "I2Cconnect.h"
#include "var.h"
#include <stdint.h>



//отправляем параметры ШИМ на AVR и записываем возвращенные значения тока и напряжения



#define DEV_REGISTERS_NUM 3


static uint16_t voltage;
static uint16_t current;
static uint8_t buf[3];
static int file;

void init_I2Cconnect(){
 
    // Open up the I2C bus
    file = open(DEVICE_I2C, O_RDWR);
    if (file == -1)
    {
        printf("Can't open I2C device\n");
        exit(1);
    }
 
    // Specify the address of the slave device.
    if (ioctl(file, I2C_SLAVE, DEV_ADDRESS) < 0)
    {
        printf("Failed to acquire bus access and/or talk to slave\n");
        exit(1);
    }
        printf("I2C device connected\n");
    }

uint16_t* get_voltage(){
    return &voltage;
}

uint16_t* get_current(){
    return &current;
}


//Send one byte and read 3 (voltage and current)
void send_byte(uint8_t msg){   

    // Write a byte to the slave.
    if (write(file, msg, 1) != 1)
    {
        printf("Failed to write to the i2c bus\n");
        exit(1);
    }
 
    // Read a bytes from the slave.
    if (read(file,buf,DEV_REGISTERS_NUM) != DEV_REGISTERS_NUM)
    {
        printf("Failed to read from the i2c bus\n");
        exit(1);
    }
    //parse

        voltage = buf[0]<<2;
        voltage = voltage ^ (buf[2]>>6);
        current = buf[1]<<2;
        current = current | (buf[2]&0x3);
}
