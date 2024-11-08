import paho.mqtt.client as mqtt

client = mqtt.Client()

# Connect to MQTT server
client.connect("mqtt_broker_ip", 1883, 60)

# Encrypt message and publish to MQTT topic
message = b"Secure MQTT message"
ephemeral_public_key, nonce, ciphertext, tag = ecies_encrypt(message, server_public_key)
client.publish("secure_topic", ephemeral_public_key + b'||' + nonce + b'||' + ciphertext + b'||' + tag)

client.disconnect()
