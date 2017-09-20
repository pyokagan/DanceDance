import sys
import socket
import base64
import time
import argparse
from Crypto.Cipher import AES


class Client:
    def __init__(self, ip_addr, port_num, secret_key):
        if len(secret_key) not in [16, 24, 32]:
            raise TypeError('AES key must be either 16, 24, or 32 bytes long')
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.iv = bytes(0 for x in range(16))
        self.secret_key = secret_key
        print('connecting to {} port {}'.format(ip_addr, port_num))
        self.sock.connect((ip_addr, port_num))

    def send(self, action, voltage, current, power, cumpower):
        cipher = AES.new(self.secret_key, AES.MODE_CBC, self.iv)
        msg = '#{action}|{voltage}|{current}|{power}|{cumpower}'.format(
                action=action, voltage=voltage, current=current,
                power=power, cumpower=cumpower)
        msg = msg.encode()
        padding = b' ' * (AES.block_size - len(msg) % AES.block_size)
        msg = padding + msg
        msg = self.iv + cipher.encrypt(msg)
        msg = base64.b64encode(msg)
        self.sock.sendall(msg)


def main(args):
    p = argparse.ArgumentParser()
    p.add_argument('--secret', default='hihihihihihihihi')
    p.add_argument('ip')
    p.add_argument('port', type=int)
    args = p.parse_args()
    client = Client(args.ip, args.port, args.secret)
    for line in sys.stdin:
        client.send(line.strip(), 300, 1000, 88, 21)


if __name__ == '__main__':
    main(sys.argv[1:])
