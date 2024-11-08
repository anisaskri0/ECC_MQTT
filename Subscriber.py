import paho.mqtt.client as mqtt

# Define the MQTT broker details
broker_address = "127.0.0.1"  # Replace with your broker's IP or hostname
broker_port = 1883  # Standard MQTT port
topic = "test/topic"  # The topic to subscribe to

# Define the callback function that will be triggered when a message is received
def on_message(client, userdata, message):
    print(f"Message received on topic {message.topic}: {message.payload.decode('utf-8')}")

# Initialize the MQTT client
client = mqtt.Client()

# Attach the message handling function to the client
client.on_message = on_message

# Connect to the broker
client.connect(broker_address, broker_port)

# Subscribe to the topic
client.subscribe(topic)

# Start the client loop to continuously monitor the broker
client.loop_start()

# Keep the program running to continuously listen for messages
print(f"Subscribed to topic '{topic}', waiting for messages...")

# Keep the script running so that it continues to listen for messages.
try:
    while True:
        pass
except KeyboardInterrupt:
    client.loop_stop()
    print("\nDisconnected from broker.")
