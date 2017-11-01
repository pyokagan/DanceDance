#!/usr/bin/python3
"""Service scripts.
"""
import sys
import argparse
import os
import os.path
import subprocess


root_dir = os.path.dirname(os.path.abspath(__file__))


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


def install(segment_size, eval_server, eval_port, eval_secret):
    systemd_path = os.path.expanduser('~/.config/systemd/user')
    os.makedirs(systemd_path, exist_ok=True)
    install_raspi_pipes(systemd_path)
    install_raspi_uart(systemd_path, segment_size)
    install_raspi_ml(systemd_path)
    install_raspi_wifi(systemd_path, eval_server, eval_port, eval_secret)
    merge_target(os.path.join(systemd_path, 'default.target'), [
        'raspi-uart.service',
        'raspi-ml.service',
        'raspi-wifi.service',
    ])
    subprocess.check_call(['systemctl', '--user', 'daemon-reload'])
    subprocess.check_call(['systemctl', '--user', 'restart', 'raspi-uart'])
    subprocess.check_call(['systemctl', '--user', 'restart', 'raspi-ml'])
    subprocess.check_call(['systemctl', '--user', 'restart', 'raspi-wifi'])


def install_raspi_pipes(systemd_path):
    with open(os.path.join(systemd_path, 'raspi-pipes.service'), 'w') as f:
        f.write('[Unit]\n')
        f.write('Description=Create FIFOs for service communication\n')
        f.write('\n')
        f.write('[Service]\n')
        f.write('Type=oneshot\n')
        f.write(''.join([
            "ExecStart=/bin/sh -c '",
            'rm -rf .raspi-service && ',
            'mkdir -p .raspi-service && ',
            'mkfifo .raspi-service/powpipe && ',
            'mkfifo .raspi-service/mlpipe && ',
            'mkfifo .raspi-service/actionpipe\'',
        ]) + '\n')
        f.write('StandardOutput=journal\n')
        f.write('StandardError=journal\n')


def install_raspi_uart(systemd_path, segment_size):
    with open(os.path.join(systemd_path, 'raspi-uart.service'), 'w') as f:
        f.write('[Unit]\n')
        f.write('Description=UART communication\n')
        f.write('Requires=raspi-pipes.service')
        f.write('\n')
        f.write('[Service]\n')
        f.write('ExecStart=' + ' '.join([
            os.path.join(root_dir, 'raspi-uart', 'raspi-uart'),
            '-t {}'.format(segment_size),
            '-p', os.path.join(root_dir, 'powpipe'),
            '-m', os.path.join(root_dir, 'mlpipe'),
        ]) + '\n')
        f.write('KillMode=mixed\n')
        f.write('Restart=always\n')
        f.write('StandardError=journal\n')
        f.write('StandardOutput=journal\n')


def install_raspi_ml(systemd_path):
    with open(os.path.join(systemd_path, 'raspi-ml.service'), 'w') as f:
        f.write('[Unit]\n')
        f.write('Description=Machine Learning\n')
        f.write('Requires=raspi-pipes.service\n')
        f.write('\n')
        f.write('[Service]\n')
        f.write('ExecStart=' + ' '.join([
            os.path.join(root_dir, 'raspi-ml.py'),
            '-o', os.path.join(root_dir, 'actionpipe'),
            os.path.join(root_dir, 'mlpipe'),
        ]) + '\n')
        f.write('KillMode=mixed\n')
        f.write('Restart=always\n')
        f.write('StandardError=journal\n')
        f.write('StandardOutput=journal\n')


def install_raspi_wifi(systemd_path, server, port, secret):
    with open(os.path.join(systemd_path, 'raspi-wifi.service'), 'w') as f:
        f.write('[Unit]\n')
        f.write('Description=Wifi communications\n')
        f.write('Requires=raspi-pipes.service\n')
        f.write('\n')
        f.write('[Service]\n')
        f.write('ExecStart=' + ' '.join([
            os.path.join(root_dir, 'raspi-eval.py'),
            '--secret',
            str(secret),
            os.path.join(root_dir, 'actionpipe'),
            os.path.join(root_dir, 'powpipe'),
            str(server),
            str(port),
        ]) + '\n')
        f.write('KillMode=mixed\n')
        f.write('Restart=always\n')
        f.write('StandardError=journal\n')
        f.write('StandardOutput=journal\n')


def main(args, prog=None):
    p = argparse.ArgumentParser(prog=prog)
    p_sub = p.add_subparsers(dest='cmd')

    p_install = p_sub.add_parser('install')
    p_install.add_argument('-t', dest='segment_size', help='Segment size',
                           type=int, default=63)
    p_install.add_argument('--server', default='192.168.43.57')
    p_install.add_argument('--port', default='8000')
    p_install.add_argument('--secret', default='hihihihihihihihi')

    args = p.parse_args(args)
    if args.cmd == 'install':
        install(args.segment_size, args.server, args.port, args.secret)
    else:
        p.print_help()


if __name__ == '__main__':
    main(sys.argv[1:], sys.argv[0])
