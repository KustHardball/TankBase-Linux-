#include <stdint.h>
#include <stdio.h>
#include "SUMD.h"
#include <pthread.h>
#include "engines.h"
#include <semaphore.h>
#include "stepper.h"
#include "var.h"
#include <sys/time.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#define TEST_SPEED 10
#define TEST_ITERATIONS 2

extern sem_t semaphore;
pthread_t test_thread;
pthread_t ch_value_thread;
uint8_t ch;
int8_t ch_value = 0;



void* test_channel(void* thread_data){
    for(int i=0;i<(CHANNELS-5)/2;i++){
        sem_wait(&semaphore);
        params.ch[i]=CENTER_POINT;
        sem_post(&semaphore);
    }
uint8_t flag = 1;
uint8_t count =0;
while (1){
    sem_wait(&semaphore);
    if(flag == 1 && params.ch[ch]+TEST_SPEED < MAX_POINT){
    params.ch[ch]+=TEST_SPEED;
    }
    else if (flag == 1){
        flag = 0;
        params.ch[ch]-=TEST_SPEED;
        count++;
    }
    else if (flag == 0 && params.ch[ch]-TEST_SPEED > MIN_POINT){
        params.ch[ch]-=TEST_SPEED;
    }
    else{
        flag = 1;
        params.ch[ch]+=TEST_SPEED;
        count++;
    }
    params.time = millis();
    sem_post(&semaphore);
    delay(10);
    if (count == TEST_ITERATIONS){
        sem_wait(&semaphore);
        params.ch[ch]=CENTER_POINT;
        sem_post(&semaphore);
        printf("Test finished\n");
        pthread_exit(NULL);
    }
}
}


void* set_value(void* thread_data){
    for(int i=0;i<(CHANNELS-5)/2;i++){
        sem_wait(&semaphore);
        params.ch[i]=CENTER_POINT;
        sem_post(&semaphore);
    }
    uint16_t val; 
    if (ch_value > 0) val = ((MAX_POINT-HIGH_POINT)/100)*ch_value + HIGH_POINT;
    else val = ((MAX_POINT-HIGH_POINT)/100)*ch_value + LOW_POINT;
    while (1){
    sem_wait(&semaphore);
    params.ch[ch]=val; 
    params.time = millis();
    sem_post(&semaphore);
}

}




void test_ch(uint8_t channel){
    ch = channel;
pthread_create(&test_thread, NULL, test_channel, NULL);
}


void set_ch_value(uint8_t channel,int8_t value){
    ch = channel;
    ch_value = value;
    if (ch_value == 0) pthread_create(&ch_value_thread, NULL, set_value, NULL);
}


void test(){
    delay(100);
    sem_wait(&semaphore);
    printf("Start test...\n");
    params.time = millis();
    params.ch[0] = 12000;
    params.ch[1] = 12000;
    params.ch[4] = 12000;
    printf("Engines 0\n");
    sem_post(&semaphore);
    delay(1000);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 12000;
    printf("Engines forward\n");
    sem_post(&semaphore);
    delay(1000);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 7710;
    params.ch[1] = 12000;
    printf("Engines back\n");
    sem_post(&semaphore);
    delay(1000);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 7710;
    params.ch[1] = 16191;
    printf("Engines right back\n");
    sem_post(&semaphore);
    delay(1000);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 12000;
    params.ch[1] = 16191;
    printf("Engines right\n");
    sem_post(&semaphore);
    delay(1000);

    printf("Start acceleration test\n");
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 13000;
    params.ch[1] = 12000;
    params.ch[4] = 16191;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 13000;
    params.ch[1] = 12000;
    params.ch[4] = 12000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 14000;
    params.ch[1] = 12000;
    params.ch[4] = 7710;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 14500;
    params.ch[1] = 12000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 14600;
    params.ch[1] = 12000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 12000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 12000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 12000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 12000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 12000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 12000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 11000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 11000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 11000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 10000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 7710;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 7710;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 12000;
    params.ch[1] = 7710;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 12000;
    params.ch[1] = 7710;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 12000;
    params.ch[1] = 7710;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 12000;
    params.ch[1] = 7710;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 12000;
    params.ch[1] = 7710;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 12000;
    params.ch[1] = 7710;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 7710;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 7710;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 11000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 11000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 11000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 12000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 13000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 13000;
    sem_post(&semaphore);
    delay(80);
    sem_wait(&semaphore);
    params.time = millis();
    params.ch[0] = 16191;
    params.ch[1] = 13000;
    sem_post(&semaphore);
    delay(80);
    printf("Engines test finish\n");
    delay(3000);
}