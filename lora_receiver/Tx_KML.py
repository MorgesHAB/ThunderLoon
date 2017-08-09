#!/usr/bin/python
#coding=utf-8
# This script will be actived each minute

# import all the module we will need
import os

os.chdir("/home/pi/lora_receiver") # Go to the recorded data folder 

os.system("cp SolarLoon.kml Result/SolarLoon2.kml")

with open("Result/SolarLoon2.kml","a") as fichier :
	print >> fichier, "\n</coordinates>\n</LineString>\n</Placemark>\n</Document>\n</kml>\n"

os.system("cp GPS/GPS_DATA.kml Result/GPS_DATA2.kml")

with open("Result/GPS_DATA2.kml","a") as fichier1 :
	print >> fichier1, "\n</coordinates>\n</LineString>\n</Placemark>\n</Document>\n</kml>\n"
