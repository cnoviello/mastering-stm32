#!/usr/bin/env python
import serial, time

SERIAL_PORT = "/dev/tty.usbmodemfd1313" #Windows users, replace with "COMx"
ser = serial.Serial(SERIAL_PORT, 38400)

while True:
    ser.write((0xff,))
    time.sleep(0.05)
    ser.write((0,))
    time.sleep(0.05)
    
ser.close()
