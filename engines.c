#include <stdio.h>
#include "engines.h"
#include <wiringPi.h>
#include <wiringSerial.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include "var.h"
#include <semaphore.h>
#include <stdlib.h>
#include "I2Cconnect.h"



int port;
pthread_t engines_thread;
extern sem_t semaphore;
uint16_t moving;
uint16_t direction;
uint32_t lasttime = 0;
uint8_t last_left_condition=0;
uint8_t last_right_condition=0;
int16_t left_cond =0;
int16_t right_cond =0;



int8_t get_pwm_value(uint16_t val){
return val/64;
}




void left_forward(uint8_t speed){
	//serialPutchar(port, speed);
	send_byte(speed);
	if(!speed){
		digitalWrite(LEFT_ENGINE_BIT2_PIN,0);
		digitalWrite(LEFT_ENGINE_BIT1_PIN,0);
	}
	digitalWrite(LEFT_ENGINE_BIT2_PIN,0);
	digitalWrite(LEFT_ENGINE_BIT1_PIN,1);
#ifdef DEBUG_MODE
	printf("LeftForward %d \n", speed);
#endif
}
void left_back(uint8_t speed){
	//serialPutchar(port, speed);
	send_byte(speed);
	if(!speed){
		digitalWrite(LEFT_ENGINE_BIT2_PIN,0);
		digitalWrite(LEFT_ENGINE_BIT1_PIN,0);
	}
	digitalWrite(LEFT_ENGINE_BIT1_PIN,0);
	digitalWrite(LEFT_ENGINE_BIT2_PIN,1);
#ifdef DEBUG_MODE
	printf("LeftBack %d \n", speed);
#endif
}
void left_stop(){
	//serialPutchar(port, 0);
	send_byte(0);
	digitalWrite(LEFT_ENGINE_BIT1_PIN,0);
	digitalWrite(LEFT_ENGINE_BIT2_PIN,0);
#ifdef DEBUG_MODE
	printf("LeftStop \n");
#endif
}
void righ_forward(uint8_t speed){
	//serialPutchar(port, speed | 0x80);
	send_byte(speed | 0x80);
	if(!speed){
		digitalWrite(RIGHT_ENGINE_BIT2_PIN,0);
		digitalWrite(RIGHT_ENGINE_BIT1_PIN,0);
	}
	digitalWrite(RIGHT_ENGINE_BIT2_PIN,0);
	digitalWrite(RIGHT_ENGINE_BIT1_PIN,1);
#ifdef DEBUG_MODE
	printf("RightForward %d \n", speed);
#endif
}
void righ_back(uint8_t speed){
	//serialPutchar(port, speed | 0x80);
	send_byte(speed | 0x80);
	if(!speed){
		digitalWrite(RIGHT_ENGINE_BIT2_PIN,0);
		digitalWrite(RIGHT_ENGINE_BIT1_PIN,0);
	}
	digitalWrite(RIGHT_ENGINE_BIT1_PIN,0);
	digitalWrite(RIGHT_ENGINE_BIT2_PIN,1);
#ifdef DEBUG_MODE
	printf("RightBack %d \n", speed);
#endif
}
void righ_stop(){
	//serialPutchar(port, 128);
	send_byte(128);
	digitalWrite(RIGHT_ENGINE_BIT1_PIN,0);
	digitalWrite(RIGHT_ENGINE_BIT2_PIN,0);
#ifdef DEBUG_MODE
	printf("RightStop \n");
#endif
}




int8_t accelerate (int8_t new, int8_t old){   //нужно передавать указатели
	if (new>old){
    	if (new>0 && old<0 && old + MOVING_ACCELERATION >0) return 0;
    	else if (new>0 && old<0 && old + MOVING_ACCELERATION <0) return old + MOVING_ACCELERATION;
    	else if (abs(new-old) > MOVING_ACCELERATION) return old + MOVING_ACCELERATION;
    	else return new;
	}
	else if (new<old){
		if (new<0 && old>0 && old - MOVING_ACCELERATION <0) return 0;
		else if (new<0 && old>0 && old - MOVING_ACCELERATION >0) return old - MOVING_ACCELERATION;
		else if (abs(old-new) > MOVING_ACCELERATION) return old - MOVING_ACCELERATION;
		else return new;
	}
}


void getmove(int8_t left, int8_t right){
	if (left>0) left_forward(left);
	else if (left<0) left_back(abs(left));
	else left_stop();
	if (right>0) righ_forward(right);
	else if (right<0) righ_back(abs(right));
	else righ_stop();
}


void set_eng_condition(){  //высчитываем состояние двигателей
if (moving >= HIGH_POINT){  //forward
	if (direction < LOW_POINT){
		left_cond = get_pwm_value((moving - CENTER_POINT) * ((double)(direction-MIN_POINT)/(double)(LOW_POINT-MIN_POINT)));
		right_cond = get_pwm_value(moving - CENTER_POINT);
	}
	else if (direction > HIGH_POINT){
		left_cond = get_pwm_value(moving - CENTER_POINT);
		right_cond = get_pwm_value((moving - CENTER_POINT) * ((double)(MAX_POINT - direction)/(double)(LOW_POINT-MIN_POINT)));
	}
	else {
		left_cond = get_pwm_value(moving - CENTER_POINT);
		right_cond = get_pwm_value(moving - CENTER_POINT);
	}
}
else if (moving <= LOW_POINT){  //back
	if (direction < LOW_POINT){
		left_cond =~get_pwm_value((LOW_POINT - moving) * ((double)(direction-MIN_POINT)/(double)(LOW_POINT-MIN_POINT)))+1;
		right_cond =~get_pwm_value(LOW_POINT - moving)+1;
	}
	else if (direction > HIGH_POINT){
		left_cond =~get_pwm_value(LOW_POINT - moving)+1;
		right_cond =~get_pwm_value((LOW_POINT - moving) * ((double)(MAX_POINT - direction)/(double)(LOW_POINT-MIN_POINT)));
	}
	else {
		left_cond =~get_pwm_value(LOW_POINT - moving)+1;
		right_cond =~get_pwm_value(LOW_POINT - moving)+1;
	}
}
else {  //разворот
	if (direction < LOW_POINT){
		left_cond =~get_pwm_value((LOW_POINT - direction) * TURN_KOEF)+1;
		right_cond =get_pwm_value((LOW_POINT - direction) * TURN_KOEF);
	}
	else if (direction > HIGH_POINT){
		left_cond =get_pwm_value((direction - HIGH_POINT) * TURN_KOEF);
		right_cond =~get_pwm_value((direction - HIGH_POINT) * TURN_KOEF)+1;
	}
	else {
		left_cond = 0;
		right_cond = 0;
	}
}
#ifdef DEBUG_MODE
printf("%d", left_cond);
printf("\n");
printf("%d", right_cond);
printf("\n");
#endif
////тут код акселерации
left_cond = accelerate (left_cond,last_left_condition);
last_left_condition = left_cond;
right_cond = accelerate (right_cond,last_right_condition);
last_right_condition = right_cond;
#ifdef DEBUG_MODE
printf("%d", left_cond);
printf("\n");
printf("%d", right_cond);
printf("\n");
#endif

getmove(left_cond,right_cond);
}








void* drive_engines(void* thread_data){
//sem_wait(&new_package); //wait when new package wil be receive
	while(1){
sem_wait(&semaphore);
if (params.time && params.time - lasttime > 0){
lasttime = params.time;
moving = params.ch[1];
direction = params.ch[0];
sem_post(&semaphore);
set_eng_condition();
}
else {
	sem_post(&semaphore);
	if (lasttime && last_right_condition!=0 && last_left_condition!=0 && millis() - lasttime > TIME_FAILSAFE){ //если нет новых пакетов, останавливаемся
		last_right_condition=0;
		last_left_condition=0;
		left_stop();
		righ_stop();
	}
	delay(TIME_DEF);
	}
}
pthread_exit(0);
}





void start_engines(int prt){
	printf("Starting engines...");
	port = prt;
	pinMode(LEFT_ENGINE_BIT1_PIN, OUTPUT);
	pinMode(LEFT_ENGINE_BIT2_PIN, OUTPUT);
	pinMode(RIGHT_ENGINE_BIT1_PIN, OUTPUT);
	pinMode(RIGHT_ENGINE_BIT2_PIN, OUTPUT);
	digitalWrite(LEFT_ENGINE_BIT1_PIN,0);
	digitalWrite(LEFT_ENGINE_BIT2_PIN,0);
	digitalWrite(RIGHT_ENGINE_BIT1_PIN,0);
	digitalWrite(RIGHT_ENGINE_BIT2_PIN,0);
	init_I2Cconnect();
	pthread_create(&engines_thread, NULL, drive_engines, NULL);
	printf("engines started\n");
}
