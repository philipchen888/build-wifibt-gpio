#!/usr/bin/python3

import bluetooth
import socket
import time
from gpio import *

host = ""
port = 1        # use port 1 for Bluetooth Communication
# Creaitng Socket Bluetooth RFCOMM communication
server = bluetooth.BluetoothSocket(bluetooth.RFCOMM)
print('Bluetooth Socket Created')
try:
        server.bind((host, port))
        print("Bluetooth Binding Completed")
except:
        print("Bluetooth Binding Failed")
server.listen(1) # One connection at a time
# Server accepts the clients request and assigns a mac address.
client, address = server.accept()
print("Connected To", address)
print("Client:", client)
try:
        while True:
            data = client.recv(1024).decode()
            if data == "start":
                    initpin(led, 'out')
                    textCommand(0x01) # clear display
                    time.sleep(.05)
                    textCommand(0x08 | 0x04) # display on, no cursor
                    textCommand(0x28) # 2 lines
                    time.sleep(.05)
                    pwm_open()
                    pwm_freq(50)
                    pwm_duty(0.05)              # min 0.05, max 0.15 180 degrees
                    pwm_polarity()
                    pwm_enable()
                    client.send("Start Test ")
            elif data == "led_on":
                    setpin(led, 1)
                    client.send("Led On ")
            elif data == "led_off":
                    setpin(led, 0)
                    client.send("Led Off ")
            elif data == "lcd_red":
                    setRGB( 255, 0, 0 )
                    setText("\n\n")
                    client.send("Lcd Red ")
            elif data == "lcd_green":
                    setRGB( 0, 255, 0 )
                    setText("\n\n")
                    client.send("Lcd Green ")
            elif data == "lcd_blue":
                    setRGB( 0, 0, 255 )
                    setText("\n\n")
                    client.send("Lcd Blue ")
            elif data == "lcd_yellow":
                    setRGB( 255, 255, 0 )
                    setText("\n\n")
                    client.send("Lcd Yellow ")
            elif data == "lcd_cyan":
                    setRGB( 0, 255, 255 )
                    setText("\n\n")
                    client.send("Lcd Cyan ")
            elif data == "lcd_purple":
                    setRGB( 255, 0, 255 )
                    setText("\n\n")
                    client.send("Lcd Purple ")
            elif data == "lcd_black":
                    setRGB( 0, 0, 0 )
                    setText("\n\n")
                    client.send("Lcd Black ")
            elif data == "lcd_white":
                    setRGB( 255, 255, 255 )
                    setText("\n\n")
                    client.send("Lcd White ")
            elif data[:5] == "Hello":
                    setRGB( 128, 255, 0 )
                    setText( data )
                    client.send("OK ")
            elif data == "servo_middle":
                    pwm_duty(0.075)
                    client.send("Servo Middle ")
            elif data == "servo_right":
                    pwm_duty(0.05)
                    client.send("Servo Right ")
            elif data == "servo_left":
                    pwm_duty(0.1)
                    client.send("Servo Left ")
            elif data == "servo_stop":
                    pwm_duty(0.075)
                    pwm_stop()
                    client.send("Servo Stop ")
            elif data == "bye":
                    pwm_stop()
                    pwm_close()
                    closepin(led)
                    client.send("Bye ")                        
            else:
                    client.send("Type led_on or led_off ")
except:
        # Closing the client and server connection
        client.close()
        server.close()

