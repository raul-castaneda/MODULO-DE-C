from Adafruit_IO import MQTTClient
import serial
import time

ADAFRUIT_IO_USERNAME = 
ADAFRUIT_IO_KEY = 
FEEDS = ["servo1t1", "servo2t1", "servo3t2", "servo4t2"]  

# Configura tu puerto
ser = serial.Serial("COM3", 9600, timeout=1) 
time.sleep(2)

def connected(client):
    print("Conectado!")
    for feed in FEEDS:
        print(f"Suscribiendo a {feed}")
        client.subscribe(feed)

def message(client, feed_id, payload):
    print(f"{feed_id}: {payload}")
    if feed_id == "servo1t1":
        msg = f"S1:{payload}\n"
        ser.write(msg.encode())
    elif feed_id == "servo2t1":
        msg = f"S2:{payload}\n"
        ser.write(msg.encode())
    elif feed_id == "servo3t2":
        msg = f"S3:{payload}\n"
        ser.write(msg.encode())
    elif feed_id == "servo4t2":
        msg = f"S4:{payload}\n"
        ser.write(msg.encode())
client = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY)

client.on_connect = connected
client.on_message = message

client.connect()
client.loop_blocking()  
