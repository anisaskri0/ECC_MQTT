import paho.mqtt.client as mqtt



# Define the MQTT broker details

broker_address = "127.0.0.1"  # Replace with your broker's IP or hostname
broker_port = 1883  # Standard MQTT port

# Initialize the MQTT client
client = mqtt.Client()

# Connect to the broker
client.connect(broker_address, broker_port)

# Publish a message to the test/topic
for i in range(5) :
    client.publish("test/topic", "Hello, this is a test message!")
#subscribe to retrieve the message published 

print("Message published")

client.loop_forever()
