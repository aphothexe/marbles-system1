
import WIFI_CONFIG
from network_manager import NetworkManager
import uasyncio
import urequests
import time
import plasma
from plasma import plasma_stick
from machine import Pin

'''
This Plasma Stick example sets your LED strip to the current #cheerlights colour.
Find out more about the Cheerlights API at https://cheerlights.com/
'''
