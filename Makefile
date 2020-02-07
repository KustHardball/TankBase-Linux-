
all:
	gcc main.c SUMD.c engines.c stepper.c test.c I2Cconnect.c logic.c -o tank_controll -lwiringPi -lpthread -lm
