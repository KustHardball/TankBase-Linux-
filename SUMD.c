#include <stdio.h>
#include "SUMD.h"
#include <wiringPi.h>
#include <wiringSerial.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <semaphore.h>
#include "var.h"
#include <unistd.h>
#include <stdlib.h>
#include "crc.h"

#define CH 16  //количество каналов





extern sem_t semaphore;
extern uint8_t show_channel;
extern uint8_t show_rssi;
int serial_number;
pthread_t scanner_thread;
int bad_pockets=0;
int count =0;





uint8_t CRC(uint8_t* data){   //считаем контрольню сумму полученного пакета и сравниваем с пришедшей
	uint16_t pack_crc;
	pack_crc = (uint16_t)data[CHANNELS-2]<<8^data[CHANNELS-1];
	if(crc16_calc(data, CHANNELS-2)==pack_crc){
		return 1;
	}
	else return 0;
}






void parse_SUMD(uint8_t* data){    //разбираем полученные данные
static uint16_t last_value = 0;
	if (data[1]==0x01 || data[1]==0x81){
		uint16_t ch_values[CH];
		if (CRC(data)){
				bad_pockets=0;
				count++;
					for (int i =0; i< CH; i++){
					ch_values[i]=data[i*2+3]<<8;
					ch_values[i]=ch_values[i]^data[i*2+3];
					digitalWrite(LIGHT_PIN, HIGH);
					sem_wait(&semaphore);
					params.ch[i]=ch_values[i];  //pushing data to main struct
					if (i==15) params.time=millis();
					sem_post(&semaphore);
					}
			if (show_channel>0 && last_value != ch_values[show_channel-1]){
					printf("%d\n",ch_values[show_channel-1]);
					last_value = ch_values[show_channel-1];
				}
				}
				else {
		printf("Bad pocket add<bad crc>\n");
			}
	}
	else {
		bad_pockets++;
		printf("Bad pocket add<bad second byte>\n");
}
}



uint8_t rssi_check(){
	static uint16_t last_rssi= 0;
	static uint16_t rssi_val = 0;
	uint32_t microseconds;
	if (!digitalRead(RSSI_PIN)){  //джем 1
		microseconds = micros();
		while (!digitalRead(RSSI_PIN) && micros() - microseconds<35);
	}
	if (digitalRead (RSSI_PIN)){ //если уже 1, ждем начала отсчета
		while(digitalRead (RSSI_PIN));
		microseconds = micros();
	}
	if (!digitalRead(RSSI_PIN)){  //начало отсчета
		while (!digitalRead(RSSI_PIN) && micros() - microseconds<35);
		if (micros() - microseconds > 35){
			sem_wait(&semaphore);
			params.rssi = 0;
			sem_post(&semaphore);
			if(show_rssi>0 && last_rssi != 0){
				printf ("Rssi: 0\n");
				last_rssi = 0;
			}
			return 0;
		}
		else{
			rssi_val = micros() - microseconds;
			sem_wait(&semaphore);
			params.rssi = rssi_val;
			sem_post(&semaphore);
			if(show_rssi>0 && last_rssi != rssi_val){
				printf ("Rssi: %d\n", rssi_val);
				last_rssi = rssi_val;
				}
			return 1;
		}
		}
}





void* scanning_SUMD(void* thread_data){  //поток сканирования протокола с приёмника
	uint8_t data[CHANNELS];
	uint8_t index = 0;
    uint8_t solve_read =0;
    unsigned int mill=millis();
    uint8_t read_buffer[256];
    uint16_t  bytes_read = 0; 
    uint16_t i=0;
    serialFlush (serial_number);
while (1==1){
	uint32_t timers;
	uint8_t ch;
	i=0;
	//timers = micros();
	uint16_t data_lenght = serialDataAvail(serial_number);
	if(data_lenght>=32){
 	bytes_read = read(serial_number,read_buffer,data_lenght);
 	//printf("%d\n", micros() - timers);
 	delayMicroseconds(2);
 	//printf("%d\n",data_lenght);
	for (data_lenght; data_lenght>0; data_lenght--,i++){
		ch = read_buffer[i];
		if (ch == 0xA8){
			if(rssi_check()){
			index = 0;
			data[index++] = 0xA8;
			solve_read=1;
			mill = millis();
			}
			else {
				serialFlush (serial_number);
				mill = millis();
			}
		}
		else if (solve_read==1) {
			data[index++]=ch;
		}
		if (index == CHANNELS){
			parse_SUMD(data);
			serialFlush (serial_number);
			solve_read=0;
			index=0;
		}	
    }
	}
	if (millis() - mill < SUMD_FREQ){
	delay(SUMD_FREQ - (millis() - mill)); //ждем следующего пакета если закончили раньше, чем за 5мс;
	mill = millis();
	//printf("W\n");
    }
	digitalWrite(LIGHT_PIN, LOW);
}
	printf("Exit scaning thread\n");
	pthread_exit(0);
}








/*void* scanning_SUMD(void* thread_data){  //поток сканирования протокола с приёмника
	uint8_t data[CHANNELS];
	uint8_t index = 0;
    uint8_t solve_read =0;
    unsigned int mill=millis();
while (1==1){
	uint32_t timers;

	while (serialDataAvail(serial_number)){
		timers = micros();
	//	printf("%d\n", micros() - timers);
		//printf("yes\n");
		uint8_t ch = serialGetchar(serial_number);
		if (ch == 0xA8){
			if(rssi_check()){
			index = 0;
			data[index++] = 0xA8;
			solve_read=1;
			mill = millis();
			}
			else serialFlush (serial_number);
		}
		else if (solve_read==1) {
			data[index++]=ch;
		}
		if (index == CHANNELS){
			parse_SUMD(data);
			serialFlush (serial_number);
			solve_read=0;
			index=0;
		}
		//printf("%d\n", micros() - timers);
	}

	if (millis() - mill < SUMD_FREQ){
	delay(SUMD_FREQ - (millis() - mill)); //ждем следующего пакета если закончили раньше, чем за 5мс;
	mill = millis();
	//printf("W\n");
    }
	digitalWrite(LIGHT_PIN, LOW);
}
	printf("Exit scaning thread\n");
	pthread_exit(0);
}*/



int start_scan(){  //функция запуска потока работы с протколом SUMD
wiringPiSetup();
printf("Trying connect to receiver's UART...");
serial_number = serialOpen (EXT_USART_CONTROL, CONTROL_USART_BAUDRATE);
if (serial_number < 0) printf("connection to receiver's UART fail\n");
else{
pinMode(RSSI_PIN, INPUT);
pinMode(LIGHT_PIN, OUTPUT);
digitalWrite(LIGHT_PIN, HIGH);
delay(200);
digitalWrite(LIGHT_PIN, LOW);
pthread_create(&scanner_thread, NULL, scanning_SUMD, NULL);
printf("connected to receiver's UART\n");
return serial_number;
}

}


//для тестов
//A8 01 10 2E B8 24 10 2E C0 2F 08 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 96 CA
//A8 01 10 2E B8 24 10 2E C0 2F 08 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 96 CA
//A8 01 10 2E B8 24 10 2E C0 2F 10 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 80 EC
//A8 01 10 2E B8 24 10 2E C0 2F 10 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 80 EC
//A8 01 10 2E C0 24 10 2E C0 2F 10 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 8B C9
//A8 01 10 2E C0 24 10 2E C0 2F 10 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 8B C9
//A8 01 10 2E C0 24 10 2E C0 2F 10 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 8B C9
//A8 01 10 2E C0 24 10 2E C0 2F 10 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 8B C9
//A8 01 10 2E C0 24 10 2E C0 2F 08 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 9D EF
//A8 01 10 2E C0 24 10 2E C0 2F 08 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 9D EF
//A8 01 10 2E C0 24 10 2E C8 2F 08 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 36 DF
//A8 01 10 2E C0 24 10 2E C8 2F 08 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 36 DF
//A8 01 10 2E B8 24 10 2E C0 2F 10 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 80 EC
//A8 01 10 2E B8 24 10 2E C0 2F 10 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 80 EC
//A8 01 10 2E B8 24 10 2E C0 2F 10 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 80 EC
//A8 01 10 2E B8 24 10 2E C0 2F 10 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 80 EC
//A8 01 10 2E C0 24 10 2E C0 2F 08 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 9D EF
//A8 01 10 2E C0 24 10 2E C0 2F 08 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 9D EF
//A8 01 10 2E C0 24 10 2E C8 2F 08 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 36 DF
//A8 01 10 2E C0 24 10 2E C8 2F 08 3F 00 3F 00 2A D0 1E C0 1E C0 3F 00 2E E0 2E E0 34 10 34 10 34 10 34 10 36 DF
