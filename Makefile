
all:
	gcc main.c SUMD.c engines.c transmit.c stepper.c crc.c test.c I2Cconnect.c logic.c -o tank_controll -lwiringPi -lpthread -lm
