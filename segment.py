#!/usr/bin/python3
"""Segments a sample stream into non-overlapping equal-sized windows."""
import sys
import argparse


def main(args, prog=None):
    p = argparse.ArgumentParser(prog=prog)
    p.add_argument('-t', metavar='SIZE', dest='window_size',
                   type=int, default=63,
                   help='window size (default: 63)')
    p.add_argument('-o', metavar='OUTPUT', dest='output',
                   type=argparse.FileType('a'), default=sys.stdout,
                   help='output stream (default: stdout)')
    p.add_argument('input', nargs='?', type=argparse.FileType('r'),
                   default=sys.stdin,
                   help='input stream (default: stdin)')
    args = p.parse_args(args)
    window = []
    for line in args.input:
        if line.startswith('#'):
            window.clear()
        else:
            window.append(line.strip())
            if len(window) >= args.window_size:
                for window_line in window:
                    print(window_line, file=args.output)
                print('# ---', file=args.output)
                args.output.flush()
                window.clear()


if __name__ == '__main__':
    main(sys.argv[1:], sys.argv[0])
