#!/usr/bin/python3

import socket
import time

HOST = '192.168.86.161'   # The remote host
PORT = 50007              # The same port as used by the server

def sendcmd( str, delay ):
    sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((HOST, PORT))
    sock.sendall(str.encode())
    print(sock.recv(1024))
    time.sleep( delay )
    sock.close()

sendcmd("start", 1)

for i in range( 5 ):
    sendcmd("led_on", 1)
    sendcmd("led_off", 1)

for i in range( 3 ):
    sendcmd("lcd_red", 0.8)
    sendcmd("lcd_green", 0.8)
    sendcmd("lcd_blue", 0.8)
    sendcmd("lcd_yellow", 0.8)
    sendcmd("lcd_cyan", 0.8)
    sendcmd("lcd_purple", 0.8)
    sendcmd("lcd_white", 0.8)
    sendcmd("lcd_black", 0.8)

sendcmd( "Hello world !\nIt works !\n", 0.8)

for i in range( 3 ):
    sendcmd("servo_middle", 0.8)
    sendcmd("servo_right", 0.8)
    sendcmd("servo_left", 0.8)
    sendcmd("servo_middle", 0.8)
    sendcmd("servo_left", 0.8)
    sendcmd("servo_right", 0.8)
    sendcmd("servo_middle", 0.8)
    sendcmd("servo_left", 0.8)

sendcmd("servo_stop", 0.8)
sendcmd("bye", 0.0)
