
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
#include "test.h"
#include "logic.h"
#include <string.h>

#define BUFF_SIZE 20


sem_t semaphore;
uint8_t show_channel = 0;
uint8_t show_rssi =0;



void parse_command(char* buff){
    if (strncmp(buff, "test", 4)==0){
        if (atoi(buff+4) > 0 && atoi(buff+4) < 17){
            printf("Start test %d channel\n", atoi(buff+4));
            show_channel=atoi(buff+4);
            show_rssi=0;
            test_ch(atoi(buff+4)-1);
        }
        else if (strncmp(buff+5, "0", 1)==0) test();
        else printf ("Unknown test command\n");
    }


    else if (strncmp(buff, "show", 4)==0){
        if (atoi(buff+4) > 0 && atoi(buff+4) < 17){
            printf("Start show %d channel\n", atoi(buff+4));
            show_channel=atoi(buff+4);
            show_rssi=0;
        }
        else if(strncmp(buff+5, "rssi", 4)==0){
            printf("Start show rssi\n");
            show_channel=0;
            show_rssi=1;
        }
        else printf ("Unknown channel\n");
    }
    else printf("Unknown command\n");

}








int main(){
    printf("Control systems starting...\n");
    sem_init(&semaphore, 0, 1);
    int port = start_scan();
    //start_engines(port);
    stepper_start();
    logic_start(&port);
    char buff[BUFF_SIZE] = "";
    delay(100);


//interface:
    wait:
    printf("Press 'q' for exit or enter comand\n");
  //  int a;
  //  scanf("%c", &a);
    if (fgets(buff, BUFF_SIZE, stdin) == NULL) {
                printf("Unknown command\n");
                goto wait;
            }
    else {
                size_t last = strlen(buff) - 1;
                if (buff[last] == '\n')
                    buff[last] = '\0';
            }

    if (buff[0]== 'q' && buff[1]== '\0'){
        sem_destroy(&semaphore);
        return 0;
    }
    else{
        parse_command(buff);
        goto wait;
    }
}
