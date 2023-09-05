//Example code: A simple server side code, which echos back the received message. 
#include <stdio.h>  
#include <string.h>   //strlen  
#include <stdlib.h>
#include <stdbool.h>  
#include <errno.h>  
#include <unistd.h>   //close  
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <sys/stat.h>
#include <signal.h>
#include <fcntl.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include "gpio.h"

int fd;

void signal_callback_handler(int signum) {
   printf("SIGINT or CTRL-C detected. Exiting gracefully\n");
   // Terminate program
   exit(signum);
}

int main(int argc , char *argv[])   
{   
    int valread;
    int msg = 0;
    int dobreak = 0;
    int pwm_period;
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    char buf[1024] = { 0 };
    char message2[40];
    char buffer[1025];
    int sock, client;
    socklen_t opt = sizeof(rem_addr);

    signal(SIGINT, signal_callback_handler);
    fd = open( "/dev/i2c-1", O_RDWR );

    // allocate socket
    sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // bind socket to port 1 of the first available
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) 1;
    bind(sock, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

    // put socket into listening mode
    listen(sock, 1);

    //accept the incoming connection
    puts("Waiting for connections ...");

    // accept one connection
    client = accept(sock, (struct sockaddr *)&rem_addr, &opt);

    ba2str( &rem_addr.rc_bdaddr, buf );
    fprintf(stderr, "accepted connection from %s\n", buf);
    memset(buf, 0, sizeof(buf));
 
    while(1)   
    {
        buffer[0] = '\0';
        if ((valread = read(client, buffer, 1024)) <= 0)   
        {
            close(client);
            if (dobreak) {
                break;
            }
        }   
             
        //Echo back the message that came in
        else 
        {
            buffer[valread] = '\0';
            if (strcmp(buffer, "start") == 0) {
                initpin( LED, "out" );
                textCommand( fd, 0x01 );        // clear display
                usleep( 5000 );
                textCommand( fd, 0x08 | 0x04 ); // display on, no cursor
                textCommand( fd, 0x28 );        // 2 lines
                usleep( 5000 );
                pwm_open();
                pwm_period = pwm_freq( 50 );
                pwm_duty( 0.075, pwm_period );
                pwm_polarity();
                pwm_enable();
                strcpy( message2, "Start Test " );
                msg = 1;
            } else if (strcmp(buffer, "led_off") == 0) {
                setpin( LED, 0 );
                strcpy( message2, "Led Off " );
                msg = 1;
            } else if (strcmp(buffer, "led_on") == 0) {
                setpin( LED, 1 );
                strcpy( message2, "Led On " );
                msg = 1;
            } else if (strcmp(buffer, "lcd_red") == 0) {
                set_backlight( fd, 255, 0, 0 );
                setText( fd, "\n\n" );
                strcpy( message2, "Lcd Red" );
                msg = 1;
            } else if (strcmp(buffer, "lcd_green") == 0) {
                set_backlight( fd, 0, 255, 0 );
                setText( fd, "\n\n" );
                strcpy( message2, "Lcd Green" );
                msg = 1;
            } else if (strcmp(buffer, "lcd_blue") == 0) {
                set_backlight( fd, 0, 0, 255 );
                setText( fd, "\n\n" );
                strcpy( message2, "Lcd Blue" );
                msg = 1;
            } else if (strcmp(buffer, "lcd_yellow") == 0) {
                set_backlight( fd, 255, 255, 0 );
                setText( fd, "\n\n" );
                strcpy( message2, "Lcd Yellow" );
                msg = 1;
            } else if (strcmp(buffer, "lcd_cyan") == 0) {
                set_backlight( fd, 0, 255, 255 );
                setText( fd, "\n\n" );
                strcpy( message2, "Lcd Cyan" );
                msg = 1;
            } else if (strcmp(buffer, "lcd_purple") == 0) {
                set_backlight( fd, 255, 0, 255 );
                setText( fd, "\n\n" );
                strcpy( message2, "Lcd Purple" );
                msg = 1;
            } else if (strcmp(buffer, "lcd_black") == 0) {
                set_backlight( fd, 0, 0, 0 );
                setText( fd, "\n\n" );
                strcpy( message2, "Lcd Black" );
                msg = 1;
            } else if (strcmp(buffer, "lcd_white") == 0) {
                set_backlight( fd, 255, 255, 255 );
                setText( fd, "\n\n" );
                strcpy( message2, "Lcd White" );
                msg = 1;
            } else if (strncmp(buffer, "Hello", 5) == 0) {
                set_backlight( fd, 128, 255, 0 );
                setText( fd, buffer );
                strcpy( message2, "OK" );
                msg = 1;
            } else if (strcmp(buffer, "servo_middle") == 0) {
                pwm_duty( 0.075, pwm_period );
                strcpy( message2, "Servo Middle " );
                msg = 1;
            } else if (strcmp(buffer, "servo_right") == 0) {
                pwm_duty( 0.05, pwm_period );
                strcpy( message2, "Servo Right " );
                msg = 1;
            } else if (strcmp(buffer, "servo_left") == 0) {
                pwm_duty( 0.1, pwm_period );
                strcpy( message2, "Servo Left " );
                msg = 1;
            } else if (strcmp(buffer, "servo_stop") == 0) {
                pwm_stop();
                strcpy( message2, "Servo Stop " );
                msg = 1;
            } else if (strcmp(buffer, "bye") == 0) {
                strcpy( message2, "Bye " );
                msg = 1;
            } else {
                strcpy( message2, "Type led_on or led_off " );
                msg = 1;
            }  
        }   
 
        if (msg > 0) {
            msg = 0;
            write(client, message2, strlen(message2));
            if ( strcmp(message2, "Bye ") == 0 ) {
                closepin( LED );
                pwm_stop();
                pwm_close();
                dobreak = 1;
            }
        }
    }  // while 
 
    close(sock); 
    return 0;   
}
