#!/usr/bin/python3

import os
import os.path
import time
import sys
import smbus
import queue

pwm_period = 0.0
bus = smbus.SMBus(1)

# this device has two I2C addresses
DISPLAY_RGB_ADDR = 0x62
DISPLAY_TEXT_ADDR = 0x3e

led = 103
spidc = 101

def check_already_exported(pinnum):
    path = '/sys/class/gpio/gpio' + str(pinnum)
    isdir = os.path.isdir(path)
    return isdir

def initpin(pinnum, mode):
    '''
    pinnum: pin number, eg. 172, 175 etc.
    mode: pin mode, valid values: in or out
    '''
    if not check_already_exported(pinnum):
        with open('/sys/class/gpio/export', 'w') as f:
            f.write(str(pinnum))
        with open('/sys/class/gpio/gpio' + str(pinnum) + '/direction', 'w') as f:
            f.write(str(mode))

def setpin(pinnum, value):
    with open('/sys/class/gpio/gpio' + str(pinnum) + '/value', 'w') as f:
        f.write(str(value))

def getpin(pinnum):
    with open('/sys/class/gpio/gpio' + str(pinnum) + '/value', 'r') as f:
        value = f.read()
        return int(value)

def closepin(pinnum):
    with open('/sys/class/gpio/unexport', 'w') as f:
        f.write(str(pinnum))

def check_pwm_already_exported():
    path = '/sys/class/pwm/pwmchip0/pwm0'
    isdir = os.path.isdir(path)
    return isdir

def pwm_open():
    if not check_pwm_already_exported():
        os.system('sudo echo 0 > /sys/class/pwm/pwmchip0/export')

def pwm_polarity():
    os.system('sudo echo "normal" > /sys/class/pwm/pwmchip0/pwm0/polarity')

def pwm_enable():
    os.system('sudo echo 1 > /sys/class/pwm/pwmchip0/pwm0/enable')

def pwm_stop():
    os.system('sudo echo 0 > /sys/class/pwm/pwmchip0/pwm0/enable')

def pwm_close():
    os.system('sudo echo 0 > /sys/class/pwm/pwmchip0/unexport')

def pwm_freq(freq):
    global pwm_period
    pwm_period = 1000000000.0 / freq
    os.system('sudo echo ' + str(int(pwm_period)) + ' > /sys/class/pwm/pwmchip0/pwm0/period')

def pwm_duty(duty):
    dutycycle = duty * int(pwm_period)
    os.system('sudo echo ' + str(int(dutycycle)) + ' > /sys/class/pwm/pwmchip0/pwm0/duty_cycle')

# I2C LCD
# set backlight to (R,G,B) (values from 0..255 for each)
def setRGB(r,g,b):
    bus.write_byte_data(DISPLAY_RGB_ADDR,0,0)
    bus.write_byte_data(DISPLAY_RGB_ADDR,1,0)
    bus.write_byte_data(DISPLAY_RGB_ADDR,0x08,0xaa)
    bus.write_byte_data(DISPLAY_RGB_ADDR,4,r)
    bus.write_byte_data(DISPLAY_RGB_ADDR,3,g)
    bus.write_byte_data(DISPLAY_RGB_ADDR,2,b)

# send command to display (no need for external use)
def textCommand(cmd):
    bus.write_byte_data(DISPLAY_TEXT_ADDR,0x80,cmd)

# set display text \n for second line(or auto wrap)
def setText(text):
    textCommand(0x01) # clear display
    time.sleep(.05)
    textCommand(0x08 | 0x04) # display on, no cursor
    textCommand(0x28) # 2 lines
    time.sleep(.05)
    count = 0
    row = 0
    for c in text:
        if c == '\n' or count >= 16:
            count = 0
            row += 1
            if row >= 2:
                break
            textCommand(0xc0)
            if c == '\n':
                continue
        count += 1
        bus.write_byte_data(DISPLAY_TEXT_ADDR,0x40,ord(c))

