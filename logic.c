
#include <stdint.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include "var.h"
#include <sys/time.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <wiringPiSPI.h>
#include <fcntl.h>
#include <sys/stat.h>	/* S_IXXX flags */
#include <sys/types.h>	/* mode_t */
#include <unistd.h>
#include "transmit.h"

#define BUFFER_SIZE	64


pthread_t logic_thread;
extern sem_t semaphore;
static uint8_t* prt;
static struct values old_params; 


// Then you want to change channels or entity which that channels drive? you must change code HERE!
uint8_t compare_data(){
	sem_wait(&semaphore);
	if(params.time != old_params.time){
	sem_post(&semaphore);
	return 1;
	}
	else{
	sem_post(&semaphore);
	return 0;
	}
}




void* logic(void* thread_data){
if(compare_data()) tx_pocket();	
delay(PROCESSING_RATE);
}






void logic_start(uint8_t* port){
	prt=port;
	printf("Starting main logic\n");
	init_tx(prt);
	old_params = params;
	pthread_create(&logic_thread, NULL, logic, NULL);
}
	




