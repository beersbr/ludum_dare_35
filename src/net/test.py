import socket

HOST = 'localhost'
PORT = 21825
BUFSZ = 1024

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST,PORT))
data = s.recv(BUFSZ)
s.close()

print 'Got: ', repr(data)