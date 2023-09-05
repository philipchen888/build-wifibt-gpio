#include <gpio.h>

//----- LED ---------------------------------------

void initpin( int pin, char *mode )
{
    FILE *fp;
    bool isdir;
    char path[100] = "";
    char direction[100] = "";
    sprintf( path, "/sys/class/gpio/gpio%d", pin );
    sprintf( direction, "/sys/class/gpio/gpio%d/direction", pin );
    struct stat st = {0};
    if ( !stat(path, &st) ) {
        isdir = S_ISDIR( st.st_mode );
    }
    if ( !isdir ) {
        fp = fopen( "/sys/class/gpio/export", "w" );
        fprintf( fp, "%d", pin );
        fclose( fp );
        fp = fopen( direction, "w" );
        fprintf( fp, "%s", mode );
        fclose( fp );
    }
}

void setpin( int pin, int value )
{
    FILE *fp;
    char pinvalue[100] = "";
    sprintf( pinvalue, "/sys/class/gpio/gpio%d/value", pin );
    fp = fopen( pinvalue, "w" );
    fprintf( fp, "%d", value );
    fclose( fp );
}

int getpin( int pin )
{
    FILE *fp;
    int value;
    char pinvalue[100] = "";
    sprintf( pinvalue, "/sys/class/gpio/gpio%d/value", pin );
    fp = fopen( pinvalue, "r" );
    fscanf( fp, "%d", &value );
    fclose( fp );
    return ( value );
}

void closepin( int pin )
{
    FILE *fp;
    fp = fopen( "/sys/class/gpio/unexport", "w" );
    fprintf( fp, "%d", pin );
    fclose( fp );
}

//------- PWM -------------------------------------------------

void pwm_open( void )
{
    FILE *fp;
    bool isdir;
    char path[100] = "";
    strcpy( path, "/sys/class/pwm/pwmchip2/pwm0" );
    struct stat st = {0};
    if ( !stat(path, &st) ) {
        isdir = S_ISDIR( st.st_mode );
    }
    if ( !isdir ) {
        fp = fopen( "/sys/class/pwm/pwmchip2/export", "w" );
        fprintf( fp, "%d", 0 );
        fclose( fp );
    }
}

void pwm_polarity( void )
{
    FILE *fp;
    fp = fopen( "/sys/class/pwm/pwmchip2/pwm0/polarity", "w" );
    fputs( "normal", fp );
    fclose( fp );
}

void pwm_enable( void )
{
    FILE *fp;
    fp = fopen( "/sys/class/pwm/pwmchip2/pwm0/enable", "w" ); 
    fprintf( fp, "%d", 1 );
    fclose( fp );
}

void pwm_stop( void )
{
    FILE *fp;
    fp = fopen( "/sys/class/pwm/pwmchip2/pwm0/enable", "w" );
    fprintf( fp, "%d", 0 );
    fclose( fp );
}

void pwm_close( void )
{
    FILE *fp;
    fp = fopen( "/sys/class/pwm/pwmchip2/unexport", "w" );
    fprintf( fp, "%d", 0 );
    fclose( fp );
}

int pwm_freq( int freq )
{
    FILE *fp;
    int pwm_period;
    pwm_period = 1000000000.0 / freq;
    fp = fopen( "/sys/class/pwm/pwmchip2/pwm0/period", "w" ); 
    fprintf( fp, "%d", pwm_period );
    fclose( fp );
    return ( pwm_period );
}

void pwm_duty( float duty, int pwm_period )
{
    FILE *fp;
    int dutycycle;
    dutycycle = duty * pwm_period; 
    fp = fopen( "/sys/class/pwm/pwmchip2/pwm0/duty_cycle", "w" );
    fprintf( fp, "%d", dutycycle );
    fclose( fp );
}

//------- I2C --------------------------------------

void wiringPiI2CWriteReg8( int fd, int addr, int reg, int data )
{
    unsigned char buf[2] = { reg, data };

    ioctl( fd, I2C_SLAVE, addr );
    write( fd, buf, 2 );
}

void set_backlight( int fd, int r, int g, int b )
{
    wiringPiI2CWriteReg8( fd, 0x62, 0, 0 );
    wiringPiI2CWriteReg8( fd, 0x62, 1, 0 );
    wiringPiI2CWriteReg8( fd, 0x62, 8, 0xaa );
    wiringPiI2CWriteReg8( fd, 0x62, 4, r );
    wiringPiI2CWriteReg8( fd, 0x62, 3, g );
    wiringPiI2CWriteReg8( fd, 0x62, 2, b );
}

void textCommand(int fd, int cmd)
{
    wiringPiI2CWriteReg8( fd, 0x3e, 0x80, cmd );
}

void setText(int fd, char * text)
{
    int i;

    textCommand( fd, 0x01 );        // clear display
    usleep( 5000);
    textCommand( fd, 0x08 | 0x04 ); // display on, no cursor
    textCommand( fd, 0x28 );        // 2 lines
    usleep( 5000 );
    for (i=0; text[i] != '\0'; i++) {
        if (text[i] == '\n') {
            textCommand( fd, 0xc0 );
	} else {
            wiringPiI2CWriteReg8( fd, 0x3e, 0x40, text[i] );
	}
    }
}
