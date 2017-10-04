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
    with open(target_path, 'w') as f:
        f.write('[Unit]\n')
        f.write('Description=Default target\n')
        f.write('AllowIsolate=true\n')
        f.write('Requires=publiship.service\n')
    print('systemd files installed.')
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
