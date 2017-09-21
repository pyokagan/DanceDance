#!/usr/bin/python3
"""Handles communications with the evaluation server"""
import sys
import socket
import base64
import time
import argparse
import threading
import errno
import queue
from Crypto.Cipher import AES


class PowerState:
    def __init__(self):
        self.lock = threading.Lock()
        self.voltage = 0
        self.current = 0


class EvalClient:
    def __init__(self, address, secret_key):
        if len(secret_key) not in (16, 24, 32):
            raise TypeError('AES key must be either 16, 24, or 32 bytes long')
        self.iv = bytes(0 for x in range(16))
        self.address = address
        self.secret_key = secret_key
        self.sock = None

    @property
    def is_connected(self):
        return self.sock is not None

    def connect(self):
        """Attempts to connect to the server.

        Returns True if the connection was successful, False otherwise.
        """
        if self.sock:
            return True
        try:
            self.sock = socket.create_connection((ip_addr, port_num))
        except OSError as e:
            print(e, file=sys.stderr)
            return False
        return True

    def send(self, action, voltage, current, power, cumpower):
        assert self.is_connected
        cipher = AES.new(self.secret_key, AES.MODE_CBC, self.iv)
        msg = '#{action}|{voltage}|{current}|{power}|{cumpower}'.format(
                action=action, voltage=voltage, current=current,
                power=power, cumpower=cumpower)
        msg = msg.encode()
        padding = b' ' * (AES.block_size - len(msg) % AES.block_size)
        msg = padding + msg
        msg = self.iv + cipher.encrypt(msg)
        msg = base64.b64encode(msg)
        try:
            self.sock.sendall(msg)
        except OSError:
            self.
            raise


class EvalServerThread(threading.Thread):
    """Eval server communication thread"""

    def __init__(self, client, pow_state, action_queue, interval=10):
        super().__init__(name='EvalServerThread')
        self.client = client
        self.pow_state = pow_state
        self.action_queue = action_queue
        self.interval = interval  # Connection interval (in s)

    def run(self):
        while True:
            while not self.client.is_connected:
                print('connecting to', self.client.address, file=sys.stderr)
                if not self.client.connect():
                    time.sleep(self.interval)
            # Receive item
            action = self.action_queue.get()
            # Read voltage/current
            with self.pow_state.lock:
                voltage = self.pow_state.voltage
                current = self.pow_state.current
            # Try to send it to the evaluation server
            try:
                self.client.send(action=action, voltage=voltage,
                                 current=current, power=voltage*current,
                                 cumpower=9000)
            except OSError as e:
                print(e, file=sys.stderr)
            finally:
                self.action_queue.task_done()


class MlThread(threading.Thread):
    """Thread that reads from the machine-learning stream"""

    def __init__(self, filepath, action_queue):
        super().__init__(name='MlThread')
        self.filepath = filepath
        self.action_queue = action_queue

    def run(self):
        while True:
            self.run_one()

    def run_one(self):
        with open(self.filepath, 'r') as f:
            for line in f:
                try:
                    self.action_queue.put_nowait(line.strip())
                except queue.Full:
                    pass


class PowThread(threading.Thread):
    """Thread that reads from the power-measurement stream"""

    def __init__(self, filepath, pow_state):
        super().__init__(name='PowThread')
        self.filepath = filepath
        self.pow_state = pow_state

    def run(self):
        while True:
            self.run_one()

    def run_one(self):
        with open(self.filepath, 'r') as f:
            for line in f:
                voltage, current = [x.strip() for x in line.split(',')]
                voltage = int(voltage)
                current = int(current)
                with self.pow_state.lock:
                    self.pow_state.voltage = voltage
                    self.pow_state.current = current


def main(args, prog=None):
    p = argparse.ArgumentParser(prog=prog)
    p.add_argument('--secret', default='hihihihihihihihi')
    p.add_argument('mlstream', type=argparse.FileType('r'))
    p.add_argument('powstream', type=argparse.FileType('r'))
    p.add_argument('server')
    p.add_argument('port', type=int)
    args = p.parse_args()


if __name__ == '__main__':
    main(sys.argv[1:], sys.argv[0])
