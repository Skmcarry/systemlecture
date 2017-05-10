import RPi.GPIO as GPIO
import time
import random

pin = [4,17,18,27,22,23,24,25,5,6]

GPIO.setmode(GPIO.BCM)
GPIO.setup(21,GPIO.IN,pull_up_down=GPIO.PUD_DOWN)

def setup(p):
  GPIO.setup(pin[p], GPIO.OUT)

def out(p, v):
  GPIO.output(pin[p], v)

for i in range(0,len(pin)): setup(i)
for i in range(0,len(pin)): out(i, 0)

print ("You must press the button when red light")

try:
  while True:
    ran = random.randint(0,9)
    out(ran, 1);
    time.sleep(1)
    if ran==9 and GPIO.input(21)==0:
       print ("You win!")
       break;
    elif ran!=9 and GPIO.input(21)==0:
       print ("Try again!")
    out(ran, 0);

except KeyboardInterrupt:
    GPIO.cleanup()

GPIO.cleanup()
  
