#!/usr/bin/python3
"""IP Publisher.

Publishes the current wlan0 configuration to
https://nus-cg3002-9.herokuapp.com.
"""
import sys
import argparse
import os.path
import os
import subprocess
import time
import urllib.request
import urllib.parse
import json
import datetime
import traceback


last_content = None


def read_target(target_path):
    header = []
    requires = set()
    with open(target_path, 'r') as f:
        for line in f:
            line = line.strip()
            if not line:
                continue
            if line.startswith('Requires='):
                line = line[len('Requires='):]
                requires.add(line)
            else:
                header.append(line)
    return header, requires


def write_target(target_path, header, requires):
    with open(target_path, 'w') as f:
        for line in header:
            f.write(line + '\n')
        f.write('\n')
        for line in requires:
            f.write('Requires=' + line + '\n')


def merge_target(target_path, requires_add):
    try:
        header, requires = read_target(target_path)
    except FileNotFoundError:
        header = [
            '[Unit]',
            'Description=Default target',
            'AllowIsolate=true'
        ]
        requires = set()
    requires.update(requires_add)
    write_target(target_path, header, requires)


def publish():
    global last_content
    content = subprocess.check_output(['ip', 'addr'], universal_newlines=True)
    if content == last_content:
        return
    timezone = datetime.timezone(datetime.timedelta(hours=8), 'SGT')
    timenow = datetime.datetime.now(timezone)
    d = {'ipaddr': str(content), 'updated': str(timenow)}
    data = urllib.parse.urlencode(d).encode()
    print('Updating...')
    urllib.request.urlopen('https://nus-cg3002-9.herokuapp.com/dancedance', data)
    last_content = content


def monitor():
    while True:
        try:
            publish()
        except Exception:
            traceback.print_exc()
        time.sleep(10)


def install():
    systemd_path = os.path.expanduser('~/.config/systemd/user')
    os.makedirs(systemd_path, exist_ok=True)
    target_path = os.path.join(systemd_path, 'default.target')
    unit_path = os.path.join(systemd_path, 'publiship.service')
    with open(unit_path, 'w') as f:
        f.write('[Unit]\n')
        f.write('Description=publish ip\n')
        f.write('\n')
        f.write('[Service]\n')
        f.write('ExecStart={} -d\n'.format(os.path.abspath(__file__)))
        f.write('KillMode=mixed\n')
        f.write('Restart=always\n')
    merge_target(target_path, [
        'publiship.service',
    ])
    print('systemd files installed.')
    subprocess.check_call(['systemctl', '--user', 'daemon-reload'])
    subprocess.check_call(['systemctl', '--user', 'restart', 'publiship'])
    print('Make sure to run:')
    print('')
    print('  loginctl enable-linger <username>')
    print('')
    print('to allow the service to start on boot.')


def main(args, prog=None):
    p = argparse.ArgumentParser(prog=prog)
    p.add_argument('-i', '--install', action='store_true')
    p.add_argument('-d', '--daemon', action='store_true')
    args = p.parse_args(args)
    if args.install:
        install()
    elif args.daemon:
        monitor()
    else:
        publish()


if __name__ == '__main__':
    main(sys.argv[1:], sys.argv[0])
