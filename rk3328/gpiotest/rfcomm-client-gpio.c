// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h>
#include <netinet/in.h> 
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <string.h> 
#include <stdlib.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

int sendcmd( int sock, char* command, int delay )
{
    int valread;
    char buffer[1024] = {0};

    write(sock , command, strlen(command));
    valread = read( sock, buffer, 1024 );
    buffer[valread] = '\0';
    printf( "%s\n", buffer );
    sleep( delay );
    return 0;
}

int led_test( int sock )
{
    int i;

    for ( i = 0; i < 5; i++ ) {
        sendcmd( sock, "led_on", 1 );
        sendcmd( sock, "led_off", 1 );
    }
    return 0;
}

int lcd_test( int sock )
{
    int i;

    for ( i = 0; i < 3; i++ ) {
        sendcmd( sock, "lcd_red", 1 );
        sendcmd( sock, "lcd_green", 1 );
        sendcmd( sock, "lcd_blue", 1 );
        sendcmd( sock, "lcd_yellow", 1 );
        sendcmd( sock, "lcd_cyan", 1 );
        sendcmd( sock, "lcd_purple", 1 );
        sendcmd( sock, "lcd_white", 1 );
        sendcmd( sock, "lcd_black", 1 );
    }
    sendcmd( sock, "Hello world !\nIt works !\n", 1 );
    return 0;
}

int servo( int sock )
{
    int i;

    for ( i = 0; i < 3; i++ ) {
        sendcmd( sock, "servo_middle", 1 );
        sendcmd( sock, "servo_right", 1 );
        sendcmd( sock, "servo_left", 1 );
        sendcmd( sock, "servo_middle", 1 );
        sendcmd( sock, "servo_left", 1 );
        sendcmd( sock, "servo_right", 1 );
        sendcmd( sock, "servo_middle", 1 );
        sendcmd( sock, "servo_left", 1 );
    }
    sendcmd( sock, "servo_stop", 1 );
    return 0;
}

int main(int argc, char const *argv[])
{
    struct sockaddr_rc addr = { 0 };
    int sock, status;
    char dest[18] = "58:11:22:61:FF:73";

    // allocate a socket
    sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // set the connection parameters (who to connect to)
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba( dest, &addr.rc_bdaddr );

    // connect to server
    status = connect(sock, (struct sockaddr *)&addr, sizeof(addr));

    // send commands
    if( status == 0 ) {
       sendcmd( sock, "start", 1 );
       led_test( sock );
       lcd_test( sock );
       servo( sock );
       sendcmd( sock, "bye", 0 );
    }

    close( sock );
    return 0;
}
