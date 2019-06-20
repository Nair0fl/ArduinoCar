#!/usr/bin/python3

import time
from random import randint
from bluepy.btle import Scanner, DefaultDelegate, Peripheral
import RPi.GPIO as GPIO
from keypad import keypad

class ScanDelegate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)


    def HandleDiscovery(self,dev,new_dev,new_dat):
        if new_dev:
            pass
        if new_dat:
            pass


scanner = Scanner().withDelegate(ScanDelegate())
kp = keypad(columnCount = 4)

time_diff = 0
first_time = 1
try:
    devices = scanner.scan(0.35)
    periph = None
    for ii in devices:
        if ii.addr == '34:03:de:34:94:69':
            periph = Peripheral(ii.addr)
            continue
    if periph:
        chars = periph.getCharacteristics()
        peripheral = chars[6]
        time.sleep(2)
        while(True) : 
            digit = None
            while digit == None:
                digit = kp.getKey()
                # Print result
                print(digit)
                time.sleep(0.05)
            
            if digit != None:
                peripheral.write((str(digit)).encode('ascii'))

    else:
        print('Unable to find the ArduinoCar. Is it started?')

except Exception as e:
    print(e)
