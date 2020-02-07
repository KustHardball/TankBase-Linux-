#include <stdio.h>
//#include "SUMD.h"
//#include <wiringPi.h>
//#include <wiringSerial.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <semaphore.h>
#include "var.h"
#include <unistd.h>
#include <stdlib.h>
#include "crc.h"

#define MAGIC_NUMB 0xA8


extern sem_t semaphore;
static uint8_t* prt;
static uint8_t package[(PACKAGE_SIZE*2)+3];
static uint16_t buf[PACKAGE_SIZE]; //Temp buf for accelerating semaphore release.
static uint16_t crc;

void init_tx(uint8_t* port){
	prt = port;
	package[0]=MAGIC_NUMB;
}

static void update_package(){    ///Package creating! Edit here!
	sem_wait(&semaphore);
	buf[0]=params.ch[3];
	buf[1]=params.ch[4];
	buf[2]=params.ch[5];
	buf[3]=params.ch[6];
	buf[4]=params.current;
	buf[5]=params.voltage;
	buf[6]=params.rssi;
	buf[7]=params.shots;
	sem_post(&semaphore);

	for(int i=1;i<=PACKAGE_SIZE;i++){ //16bit to 2x8bit
		package[i*2-1]=buf[i-1]&0xFF;
		package[i*2]=buf[i-1]>>8;
	}

	crc = crc16_calc(package+1, PACKAGE_SIZE*2);
	package[PACKAGE_SIZE-2]=crc&0xFF;
	package[PACKAGE_SIZE-1]=crc>>8;

}

static void send_package(){
	write(*prt, buf, (PACKAGE_SIZE*2)+3);
}

void tx_pocket(){
	update_package();
	send_package();
	#ifdef DEBUG_MODE
	printf("Package send to tower\n");
	#endif
}