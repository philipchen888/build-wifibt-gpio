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

#define HOST "192.168.86.163"
#define PORT 8888

int make_socket( uint16_t port )
{
    int sock;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons( port );

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, HOST, &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    return sock;
}

int sendcmd( char* command, int delay )
{
    int sock;
    int valread;
    char buffer[1024] = {0};

    sock = make_socket( PORT );
    send(sock , command, strlen(command) , 0 );
    valread = read( sock, buffer, 1024 );
    buffer[valread] = '\0';
    printf( "%s\n", buffer );
    sleep( delay );
    close(sock);
    return 0;
}

int led_test()
{
    int i;

    for ( i = 0; i < 5; i++ ) {
        sendcmd( "led_on", 1 );
        sendcmd( "led_off", 1 );
    }
    return 0;
}

int lcd_test()
{
    int i;

    for ( i = 0; i < 3; i++ ) {
        sendcmd( "lcd_red", 1 );
        sendcmd( "lcd_green", 1 );
        sendcmd( "lcd_blue", 1 );
        sendcmd( "lcd_yellow", 1 );
        sendcmd( "lcd_cyan", 1 );
        sendcmd( "lcd_purple", 1 );
        sendcmd( "lcd_white", 1 );
        sendcmd( "lcd_black", 1 );
    }
    sendcmd( "Hello world !\nIt works !\n", 1 );
    return 0;
}

int servo()
{
    int i;

    for ( i = 0; i < 3; i++ ) {
        sendcmd( "servo_middle", 1 );
        sendcmd( "servo_right", 1 );
        sendcmd( "servo_left", 1 );
        sendcmd( "servo_middle", 1 );
        sendcmd( "servo_left", 1 );
        sendcmd( "servo_right", 1 );
        sendcmd( "servo_middle", 1 );
        sendcmd( "servo_left", 1 );
    }
    sendcmd( "servo_stop", 1 );
    return 0;
}

int main(int argc, char const *argv[])
{
    sendcmd( "start", 1 );

    led_test();
    lcd_test();
    servo();

    sendcmd( "bye", 1 );
    return 0;
}
