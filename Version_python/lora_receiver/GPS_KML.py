#!/usr/bin/python
#coding=utf-8
# This script will be actived each minute

# import all the module we will need
import gps
import time
import os
import RPi.GPIO as GPIO

# Define vrariables how would count the number of GPS Data
Nbr_GPS_Data = 0

# Define the number of Data you want to record in 1 min
Time_between_each_recorded_data =  10 #  in second
Check_if_all_msg = False
Nbr_received_DATA = 0

# Setup the LoRa GPS Hat    
# Read on the localhost 2947 port (gpsd)
session = gps.gps("localhost", "2947")
session.stream(gps.WATCH_ENABLE | gps.WATCH_NEWSTYLE)

try :   
  while True:
     report = session.next()   # Wait the next TPV report
     # Uncomment the next line to see all the data on the TPV report
     #print report
     # The GPS takes GPS data every secondes, so we take only the GPS data
     # every "x" secondes
     if report['class'] == 'TPV':
      # if there's GPS time data
      if hasattr(report, 'time' and 'speed' and 'lon' and 'lat' and 'alt'): 
        if Nbr_GPS_Data % Time_between_each_recorded_data == 0 : # % = the rest of the division
           # We want to be sure that we receive all the GPS data !
           # We will have to change the type of GPS Data in structure type
           if Check_if_all_msg == False :
              if hasattr(report, 'time') :
                GPSTIME = str(report.time)
                TIME_PI0 = time.strftime("%H-%M-%S")    # take the Pi0 time 
                Nbr_received_DATA +=1
              if hasattr(report, 'speed') :
                SPEED = str(report.speed * gps.MPS_TO_KPH)
                Nbr_received_DATA +=1
              if hasattr(report, 'lon') :
                LONGITUDE = str(report.lon)
                Nbr_received_DATA +=1
              if hasattr(report, 'lat') :
                LATITUDE = str(report.lat)
                Nbr_received_DATA +=1
              if hasattr(report, 'alt') :
                ALTITUDE = str(report.alt)
                Nbr_received_DATA +=1
              if Nbr_received_DATA == 5 :
                Check_if_all_msg = True
                Nbr_received_DATA = 0
              if Nbr_received_DATA != 5 :
                Nbr_received_DATA = 0
              if Check_if_all_msg == True :
                 os.chdir("/home/pi/lora_receiver/GPS") # Go to the recorded data folder 
                 with open("Time_Alt.txt","a") as fichier :
                    print >> fichier, "AT ",GPSTIME,", vitesse: ",SPEED,", Altitude: ",ALTITUDE   # record time on file.txt
                 with open("GPS_DATA.kml","a") as fichier1 :
		                GPS_DATA=LONGITUDE+","+LATITUDE
                    print >> fichier1, GPS_DATA
                 os.chdir("/home/pi/lora_receiver") # Go to the recorded data folder 
                 os.system("cp SolarLoon.kml Result/SolarLoon2.kml")
                 with open("Result/SolarLoon2.kml","a") as fichier :
                    print >> fichier, "\n</coordinates>\n</LineString>\n</Placemark>\n</Document>\n</kml>\n"
                 os.system("cp GPS/GPS_DATA.kml Result/GPS_DATA2.kml")
                 with open("Result/GPS_DATA2.kml","a") as fichier1 :
                    print >> fichier1, "\n</coordinates>\n</LineString>\n</Placemark>\n</Document>\n</kml>\n"   
                 Check_if_all_msg = False
        Nbr_GPS_Data +=1

except KeyError:
   pass
except KeyboardInterrupt:
  GPIO.cleanup()
  quit()
except StopIteration:
   session = None
   print "GPSD has terminated)"
