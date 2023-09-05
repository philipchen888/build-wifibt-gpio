#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>   //close
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/spi/spidev.h>

#define LED     163

void initpin( int pin, char *mode );
void setpin( int pin, int value );
int getpin( int pin );
void closepin( int pin );
void pwm_open( void );
void pwm_polarity( void );
void pwm_enable( void );
void pwm_stop( void );
void pwm_close( void );
int pwm_freq( int freq );
void pwm_duty( float duty, int pwm_period );
void wiringPiI2CWriteReg8( int fd, int addr, int reg, int data );
void set_backlight( int fd, int r, int g, int b );
void textCommand(int fd, int cmd);
void setText(int fd, char * text);
