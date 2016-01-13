#!/usr/bin/env python
import serial, time

SERIAL = "/dev/tty.usbmodem1a1213" #Windows users, replace with "COMx"
ser = serial.Serial(SERIAL, 38400)

while True:
    ser.write((0xff,))
    time.sleep(0.05)
    ser.write((0,))
    time.sleep(0.05)
    
ser.close()
