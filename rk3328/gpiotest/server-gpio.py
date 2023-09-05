#!/usr/bin/python3

import select
import socket
import time
from gpio import *

HOST = ''                 # Symbolic name meaning all available interfaces
PORT = 50007              # Arbitrary non-privileged port

# Create a TCP/IP socket
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server.setblocking(0)

# Bind the socket to the port
server.bind((HOST, PORT))

# Listen for incoming connections
server.listen(5)

# Sockets from which we expect to read
inputs = [ server ]

# Sockets to which we expect to write
outputs = [ ]

# Outgoing message queues (socket:queue)
message_queues = {}

while inputs:

    # Wait for at least one of the sockets to be ready for processing
    readable, writable, exceptional = select.select(inputs, outputs, [])

    # Handle inputs
    for s in readable:

        if s is server:
            # A "readable" server socket is ready to accept a connection
            connection, client_address = s.accept()
            connection.setblocking(0)
            inputs.append(connection)

            # Give the connection a queue for data we want to send
            message_queues[connection] = queue.Queue()
        else:
            data = s.recv(1024).decode()
            if data:
                # A readable client socket has data
                # print data
                # message_queues[s].put(data)
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
                        message_queues[s].put("Start Test ")
                elif data == "led_on":
                        setpin(led, 1)
                        message_queues[s].put("Led On ")
                elif data == "led_off":
                        setpin(led, 0)
                        message_queues[s].put("Led Off ")
                elif data == "lcd_red":
                        setRGB( 255, 0, 0 )
                        setText("\n\n")
                        message_queues[s].put("Lcd Red ")
                elif data == "lcd_green":
                        setRGB( 0, 255, 0 )
                        setText("\n\n")
                        message_queues[s].put("Lcd Green ")
                elif data == "lcd_blue":
                        setRGB( 0, 0, 255 )
                        setText("\n\n")
                        message_queues[s].put("Lcd Blue ")
                elif data == "lcd_yellow":
                        setRGB( 255, 255, 0 )
                        setText("\n\n")
                        message_queues[s].put("Lcd Yellow ")
                elif data == "lcd_cyan":
                        setRGB( 0, 255, 255 )
                        setText("\n\n")
                        message_queues[s].put("Lcd Cyan ")
                elif data == "lcd_purple":
                        setRGB( 255, 0, 255 )
                        setText("\n\n")
                        message_queues[s].put("Lcd Purple ")
                elif data == "lcd_black":
                        setRGB( 0, 0, 0 )
                        setText("\n\n")
                        message_queues[s].put("Lcd Black ")
                elif data == "lcd_white":
                        setRGB( 255, 255, 255 )
                        setText("\n\n")
                        message_queues[s].put("Lcd White ")
                elif data[:5] == "Hello":
                        setRGB( 128, 255, 0 )
                        setText( data )
                        message_queues[s].put("OK ")
                elif data == "servo_middle":
                        pwm_duty(0.075)
                        message_queues[s].put("Servo Middle ")
                elif data == "servo_right":
                        pwm_duty(0.05)
                        message_queues[s].put("Servo Right ")
                elif data == "servo_left":
                        pwm_duty(0.1)
                        message_queues[s].put("Servo Left ")
                elif data == "servo_stop":
                        pwm_duty(0.075)
                        pwm_stop()
                        message_queues[s].put("Servo Stop ")
                elif data == "bye":
                        message_queues[s].put("Bye ")                        
                else:
                        message_queues[s].put("Type led_on or led_off ")
                # Add output channel for response
                if s not in outputs:
                    outputs.append(s)
            else:
                # Interpret empty result as closed connection
                # Stop listening for input on the connection
                if s in outputs:
                    outputs.remove(s)
                inputs.remove(s)
                s.close()

                # Remove message queue
                del message_queues[s]

    # Handle outputs
    for s in writable:
        try:
            next_msg = message_queues[s].get_nowait()
        except queue.Empty:
            # No messages waiting so stop checking for writability.
            outputs.remove(s)
        else:
            s.send(next_msg.encode())
            if next_msg == "Bye ":
                pwm_stop()
                pwm_close()
                closepin(led)
                outputs.clear()
                inputs.clear()
