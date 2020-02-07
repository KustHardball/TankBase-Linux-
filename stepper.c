#include <stdio.h>
#include "stepper.h"
#include <wiringPi.h>
#include <wiringSerial.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <semaphore.h>
#include "var.h"
#include <stdlib.h>
#include <math.h>

extern sem_t semaphore;
sem_t stepper_speed_semaphore;

pthread_t stepper_thread;
int32_t speed=0;
pthread_t stepper_move_thread;
uint16_t servo_value;



int32_t map(int32_t x, int16_t diapazone){
	return (x - 1) * (-999) / (diapazone)+ 1000;
}

int32_t map_root (int32_t x){
	return (x-1)*(1000- 1) / (1000000- 1) + 1;
}


int32_t formula (int16_t val){
	if (val > 0 ){
		return map_root((int32_t)pow((double)val,2));
		}
	return 0;
}



int32_t speed_calc(int16_t entry_speed){  //scaling speed because time between steps has quadratic dependens of rotate speed
return MIN_DURATION+(((MAX_DURATION-MIN_DURATION) / 1000)*(formula(entry_speed-1)));
}



void move_stepper(uint16_t* val){
if (*val > HIGH_POINT){
sem_wait(&stepper_speed_semaphore);
speed=speed_calc(map(*val-HIGH_POINT, STEPPER_MAX_POINT - HIGH_POINT));
sem_post(&stepper_speed_semaphore);
}
else if(*val < LOW_POINT){
sem_wait(&stepper_speed_semaphore);
speed=~speed_calc(map(LOW_POINT+1 - *val, LOW_POINT - STEPPER_MIN_POINT))+1;
sem_post(&stepper_speed_semaphore);
}
else {
	sem_wait(&stepper_speed_semaphore);
	speed = 0;
	sem_post(&stepper_speed_semaphore);
}
#ifdef DEBUG_MODE
	printf("Speed = ");
	sem_wait(&stepper_speed_semaphore);
	printf("%d",speed);
	sem_post(&stepper_speed_semaphore);
	printf("\n");
#endif
}


void  dostep(int32_t* duration) {
  //printf("%d\n",*duration );
	static uint8_t position = 0;
	if (*duration==0){
		digitalWrite(STEPPER_PIN1, LOW);
		digitalWrite(STEPPER_PIN2, LOW);
		digitalWrite(STEPPER_PIN3, LOW);
		digitalWrite(STEPPER_PIN4, LOW);
		return;
	}

	switch (position)
	{
	case 0:
	{
		digitalWrite(STEPPER_PIN1, HIGH);
		digitalWrite(STEPPER_PIN2, HIGH);
		digitalWrite(STEPPER_PIN3, LOW);
		digitalWrite(STEPPER_PIN4, LOW);
		if (*duration>0)
		{
			position = 1;
		}
		else
		{
			position = 3;
		}

			break;
		}
	case 1:
	{
		digitalWrite(STEPPER_PIN1, LOW);
		digitalWrite(STEPPER_PIN2, HIGH);
		digitalWrite(STEPPER_PIN3, HIGH);
		digitalWrite(STEPPER_PIN4, LOW);
		if (*duration>0)
		{
			position = 2;
		}
		else
		{
			position = 0;
		}
			break;
		}
	case 2:
	{
		digitalWrite(STEPPER_PIN1, LOW);
		digitalWrite(STEPPER_PIN2, LOW);
		digitalWrite(STEPPER_PIN3, HIGH);
		digitalWrite(STEPPER_PIN4, HIGH);
		if (*duration>0)
		{
			position = 3;
		}
		else
		{
			position = 1;
		}
			break;
		}
	case 3:
	{
		digitalWrite(STEPPER_PIN1, HIGH);
		digitalWrite(STEPPER_PIN2, LOW);
		digitalWrite(STEPPER_PIN3, LOW);
		digitalWrite(STEPPER_PIN4, HIGH);
		if (*duration>0)
		{
			position = 0;
		}
		else
		{
			position = 2;
		}
			break;
		}
	}
  delayMicroseconds(MIN_DURATION);
  digitalWrite(STEPPER_PIN1, LOW);
  digitalWrite(STEPPER_PIN2, LOW);
  digitalWrite(STEPPER_PIN3, LOW);
  digitalWrite(STEPPER_PIN4, LOW);
  if(abs(*duration)>=MIN_DURATION){
	delayMicroseconds(abs(*duration)-MIN_DURATION);
}
}




void* rotate (void* thread_data){
	int32_t local_speed;
	uint8_t last=1;
	while(1){
		sem_wait(&stepper_speed_semaphore);
		local_speed = speed;
		sem_post(&stepper_speed_semaphore);
		if (local_speed==0 && last) {
			dostep(&local_speed);
			last=0;
#ifdef DEBUG_MODE
			printf ("Stop steps\n");
#endif
			delay(STEPPER_FREQ);
		}
		else if (local_speed!=0){
			if(!last) last=1;
			dostep(&local_speed);
#ifdef DEBUG_MODE
			printf ("Step\n");
#endif
		}
		else delay (STEPPER_FREQ);
	}
}




void* drive_stepper(void* thread_data){
uint32_t lasttime = 0;
uint32_t check_time = millis();
uint16_t rc_value = CENTER_POINT;
pthread_create(&stepper_move_thread, NULL, rotate, NULL);
	while(1){
sem_wait(&semaphore);
if (params.time && params.time - lasttime > 0){
lasttime = params.time;
rc_value=params.ch[3];
servo_value = params.ch[4];
sem_post(&semaphore);
check_time = millis();
move_stepper(&rc_value);
}
else {
	sem_post(&semaphore);
	if (lasttime && millis() - lasttime > BRAKE_TIME){ //если нет новых пакетов, останавливаемся
	sem_wait(&stepper_speed_semaphore);
	speed=0;
	sem_post(&stepper_speed_semaphore);
	}
	}
if (millis()-check_time < STEPPER_FREQ){
	delay (STEPPER_FREQ -(millis()-check_time));
	check_time =millis();
}
}
pthread_exit(0);
}












void stepper_start(){
	printf("Stepper initialization...");
	sem_init(&stepper_speed_semaphore, 0, 1);
	pinMode(STEPPER_PIN1, OUTPUT);
	pinMode(STEPPER_PIN2, OUTPUT);
	pinMode(STEPPER_PIN3, OUTPUT);
	pinMode(STEPPER_PIN4, OUTPUT);
	digitalWrite(STEPPER_PIN1, LOW);
	digitalWrite(STEPPER_PIN2, LOW);
	digitalWrite(STEPPER_PIN3, LOW);
	digitalWrite(STEPPER_PIN4, LOW);
	pthread_create(&stepper_thread, NULL, drive_stepper, NULL);
 	printf("stepper started\n");
}
