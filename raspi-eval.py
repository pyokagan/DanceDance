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
import traceback
import os
from Crypto.Cipher import AES
from Crypto import Random


class PowerState:
    def __init__(self):
        self.lock = threading.Lock()
        self.voltage = 0.0  # voltage (in V)
        self.current = 0.0  # current (in A)
        self.cumpower = 0


class EvalClient:
    def __init__(self, address, secret_key):
        if len(secret_key) not in (16, 24, 32):
            raise TypeError('AES key must be either 16, 24, or 32 bytes long')
        rndfile = Random.new()
        self.iv = rndfile.read(16)
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
            self.sock = socket.create_connection(self.address)
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
            self.sock = None
            raise


class EvalServerThread(threading.Thread):
    """Eval server communication thread"""

    def __init__(self, client, pow_state, action_queue, interval=10):
        super().__init__(name='EvalServerThread', daemon=True)
        self.client = client
        self.pow_state = pow_state
        self.action_queue = action_queue
        self.interval = interval  # Connection interval (in s)

    def run(self):
        try:
            self._run()
        except:
            traceback.print_exc()
            os._exit(1)

    def _run(self):
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
                                 cumpower=self.pow_state.cumpower)
            except OSError as e:
                print(e, file=sys.stderr)
            finally:
                self.action_queue.task_done()


class MlThread(threading.Thread):
    """Thread that reads from the machine-learning stream"""

    def __init__(self, filepath, action_queue):
        super().__init__(name='MlThread', daemon=True)
        self.filepath = filepath
        self.action_queue = action_queue

    def run(self):
        try:
            while True:
                self.run_one()
        except:
            traceback.print_exc()
            os._exit(1)

    def run_one(self):
        print('opening', self.filepath, '...', file=sys.stderr)
        with open(self.filepath, 'r') as f:
            print('opened', self.filepath)
            for line in f:
                try:
                    self.action_queue.put_nowait(line.strip())
                except queue.Full:
                    pass


class PowThread(threading.Thread):
    """Thread that reads from the power-measurement stream"""

    def __init__(self, filepath, pow_state):
        super().__init__(name='PowThread', daemon=True)
        self.filepath = filepath
        self.pow_state = pow_state

    def to_current(self, sensor_value):
        "Converts the raw sensor value to current (in A)"

        VOLTAGE_REF = 5
        RS = 0.1  # Shunt resistor value (in ohms)

        # Remap the ADC value into a voltage number
        sensor_value = (sensor_value * VOLTAGE_REF) / 1023

        # Follow the equation given by the INA169 datasheet to determine
        # the current flowing through RS. Assume RL = 10k
        # Is = (Vout * 1k) / (RS * RL)
        return sensor_value / (10 * RS)


    def to_voltage(self, sensor_value):
        "Convert the raw potential sensor value to voltage (in V)"

        VOLTAGE_REF = 5.0
        return min(2 * (sensor_value * VOLTAGE_REF) / 1023, VOLTAGE_REF)


    def run(self):
        try:
            while True:
                self.run_one()
        except:
            traceback.print_exc()
            os._exit(1)

    def run_one(self):
        print('opening', self.filepath, '...', file=sys.stderr)
        with open(self.filepath, 'r') as f:
            print('opened', self.filepath, file=sys.stderr)
            for line in f:
                raw_voltage, raw_current = [x.strip() for x in line.split(',')]
                raw_voltage = int(raw_voltage)
                raw_current = int(raw_current)
                with self.pow_state.lock:
                    self.pow_state.voltage = self.to_voltage(raw_voltage)
                    self.pow_state.current = self.to_current(raw_current)


class CumpowerThread(threading.Thread):
    """Thread that updates cumpower"""

    def __init__(self, pow_state):
        super().__init__(name='CumpowerThread', daemon=True)
        self.pow_state = pow_state

    def run(self):
        try:
            self._run()
        except:
            traceback.print_exc()
            os._exit(1)

    def _run(self):
        while True:
            with self.pow_state.lock:
                self.pow_state.cumpower += (self.pow_state.voltage *
                        self.pow_state.current)
            time.sleep(1)


def main(args, prog=None):
    p = argparse.ArgumentParser(prog=prog)
    p.add_argument('--secret', default='hihihihihihihihi')
    p.add_argument('mlstream')
    p.add_argument('powstream')
    p.add_argument('server')
    p.add_argument('port', type=int)
    args = p.parse_args()
    wifi_client = EvalClient((args.server, args.port), args.secret)
    pow_state = PowerState()
    action_queue = queue.Queue(1)
    wifi_thread = EvalServerThread(wifi_client, pow_state, action_queue)
    ml_thread = MlThread(args.mlstream, action_queue)
    pow_thread = PowThread(args.powstream, pow_state)
    cumpower_thread = CumpowerThread(pow_state)
    wifi_thread.start()
    ml_thread.start()
    pow_thread.start()
    cumpower_thread.start()
    wifi_thread.join()
    ml_thread.join()
    pow_thread.join()
    cumpower_thread.join()


if __name__ == '__main__':
    main(sys.argv[1:], sys.argv[0])
