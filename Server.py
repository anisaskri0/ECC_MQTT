from Crypto.Cipher import AES , PKCS1_OAEP
from Crypto.PublicKey import RSA 
import socket 
import os
def decrypt_file(encrypted_aes_key , nonce , tag,ciphertext , private_key_path) :
    
    # reach the server's private key
    with open ("server_private.pem" , "rb") as f :
        private_key=RSA.import_key(f.read())
    #decrypt the aes key using rsa private key
    cipher_rsa = PKCS1_OAEP.new(private_key)
    aes_key = cipher_rsa.decrypt(encrypted_aes_key)
    #decrypt the data using AES key
    cipher_aes = AES.new(aes_key , AES.MODE_EAX , nonce=nonce)
    decrypted_data = cipher_aes.decrypt_and_verify(ciphertext,tag)
    return decrypted_data
def handle_client (client_socket) : 
    #recerive encrypted data from client 
    data = client_socket.recv(4096)
    encrypted_aes_key , nonce ,tag ,ciphertext=data.split(b'||')
    #decrypt the data 
    decrypted_data= decrypt_file(encrypted_aes_key, nonce, tag , ciphertext , 'server_private.pem')
    #save the decrypted file 
    with open('received_file' , 'wb') as f:
        f.write(decrypted_data)
    print("file received and decrypted")
# Start the server
def start_server(server_ip, server_port):    
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((server_ip, server_port))
        server_socket.listen()
        print(f"Server listening on {server_ip}:{server_port}")
        
        while True:
            client_socket, _ = server_socket.accept()
            handle_client(client_socket)
            client_socket.close()

if __name__ == "__main__":
    server_ip = "127.0.0.1"
    server_port = 5000
    start_server(server_ip, server_port)
