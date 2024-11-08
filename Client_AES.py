from Crypto.Cipher import AES , PKCS1_OAEP
from Crypto.PublicKey import RSA 
import socket 
import os
def encrypt_file(file_path, server_public_key):
    # Create AES key and cipher
    aes_key = os.urandom(16)
    cipher_aes = AES.new(aes_key, AES.MODE_EAX)
    
    # Encrypt the file data
    with open(file_path, 'rb') as f:
        file_data = f.read()
        ciphertext, tag = cipher_aes.encrypt_and_digest(file_data)
    
    # Encrypt the AES key using the server's public RSA key
    rsa_key = RSA.import_key(server_public_key)
    cipher_rsa = PKCS1_OAEP.new(rsa_key)
    encrypted_aes_key = cipher_rsa.encrypt(aes_key)
    
    return encrypted_aes_key, cipher_aes.nonce, tag, ciphertext  # Corrected order

def send_file(file_path, server_ip, server_port):
    # Load the server's public key
    with open("server_public.pem", "rb") as f:
        server_public_key = f.read()
    
    # Encrypt the file
    encrypted_aes_key, nonce, tag, ciphertext = encrypt_file(file_path, server_public_key)
    
    # Connect to the server and send data
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((server_ip, server_port))
        
        # Send the encrypted AES key, nonce, tag, and encrypted file data
        s.sendall(encrypted_aes_key + b'||' + nonce + b'||' + tag + b'||' + ciphertext)
    
    print(f"Encrypted file '{file_path}' sent to the server.")



if __name__ == "__main__":
    file_path = "test.txt"
    server_ip = "127.0.0.1"
    server_port = 5000
    send_file(file_path, server_ip, server_port)