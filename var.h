
//#define DEBUG_MODE


#define LEFT_ENGINE_BIT1_PIN 22
#define LEFT_ENGINE_BIT2_PIN 23
#define RIGHT_ENGINE_BIT1_PIN 24
#define RIGHT_ENGINE_BIT2_PIN 25
#define STEPPER_PIN1 26
#define STEPPER_PIN2 27
#define STEPPER_PIN3 28
#define STEPPER_PIN4 29
#define RSSI_PIN 1
#define LIGHT_PIN 6
#define CAM_PIN 4
#define ENGINE_ACCELERATE_PIN 5
#define ADC_PIN 7
#define ACCELEROMETER_PIN 0


//channels values
#define MAX_POINT 16191
#define MIN_POINT 7710
#define CENTER_POINT 11950
#define LOW_POINT 11307
#define HIGH_POINT 12594

//engines thread ariability
#define TIME_DEF 50 //sampling freq ms
#define TURN_KOEF 0.4  //turning speed coef
#define TIME_FAILSAFE 300
#define MOVING_ACCELERATION 1 // 1-65


//stepper thread variability
#define BRAKE_TIME 300  //ms failsafe
#define MIN_DURATION 700  //mks
#define MAX_DURATION 100000  //mks
#define STEPPER_FREQ 40 //sampling freq ms

//connect to AVR device vith voltage and curent ADC and engines PWM
#define DEV_ADDRESS 0x10
#define DEVICE_I2C "/dev/i2c-2"


struct values {
uint16_t ch[16];
uint32_t time;
uint16_t current;
uint16_t voltage;
uint16_t rssi;
uint16_t shots;
} params;
