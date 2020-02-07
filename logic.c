
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

#define SPI_DESCRIPTOR "/dev/spidev0.0"
#define BUFFER_SIZE	64


pthread_t logic_thread;
extern sem_t semaphore;






void* logic(void* thread_data){
	int res;
	char buffer[BUFFER_SIZE];
	buffer[0]='f';
	res = open(SPI_DESCRIPTOR,O_RDWR);
	if (res == -1) printf("SPI ERROR!\n");
	else{
		printf("SPI opened\n");		
		for (int i =0; i <1000; i++){
			 write(res, buffer, 1);
			 //printf ("%c\n",buffer[0]);
			}
		}



	//res = wiringPiSPISetup (0, 500000);
	//if (res>=0) printf("SPI bus connected to descriptor: %d\n", res);
	//else printf("SPI ERROR!\n");


}






void logic_start(){
	printf("Starting main logic\n");
	pthread_create(&logic_thread, NULL, logic, NULL);
}
	




