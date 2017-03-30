import httplib, urllib
import time
import math
 
def thermometer():
        while True:
                tempfile = open("/sys/bus/w1/devices/28-0000064d69fb/w1_slave")
                thetext = tempfile.read()
                tempfile.close()
                tempdata = thetext.split("\n")[1].split(" ")[9]
                temperature = float(tempdata[2:])
                temperature = temperature / 1000
                print temperature
                temp = temperature
                params = urllib.urlencode({'field1': temp, 'key':'XDNMOO2WVVPD8OY0'})     # use your API key generated in the thingspeak channels for the value of 'key'
                # temp is the data you will be sending to the thingspeak channel for plotting the graph. You can add more than one channel and plot more graphs
                headers = {"Content-typZZe": "application/x-www-form-urlencoded","Accept": "text/plain"}
                conn = httplib.HTTPConnection("api.thingspeak.com:80")
                try:
                   conn.request("POST", "/update", params, headers)
                   response = conn.getresponse()
                   print temp
                   temp = temperature
                   params = urllib.urlencode({'field1': temp, 'key':'XDNMOO2WVVPD8OY0'})     # use your API key generated in the thingspeak channels for the value of 'key'
                # temp is the data you will be sending to the thingspeak channel for plotting the graph. You can add more than one channel and plot more graphs
                headers = {"Content-typZZe": "application/x-www-form-urlencoded","Accept": "text/plain"}
                conn = httplib.HTTPConnection("api.thingspeak.com:80")
                try:
                   conn.request("POST", "/update", params, headers)
                   response = conn.getresponse()
                   print temp
                   print response.status, response.reason
                   data = response.read()
                   conn.close()
                except:
                 print "connection failed"
 
#sleep for 16 seconds (api limit of 15 secs)
if __name__ == "__main__":
        while True:
                thermometer()


