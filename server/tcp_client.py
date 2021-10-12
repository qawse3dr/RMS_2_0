import socket
import sys

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = ('127.0.0.1', 1234)
sock.connect(server_address)

#try:
while( True):
    sock.send(b'a')

#except:
#    pass

sock.close()