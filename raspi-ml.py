#!/usr/bin/python3
"""Raspi machine learning process"""
import sys
import argparse
import collections
import time


Sample = collections.namedtuple('Sample', [
    'acc1x',
    'acc1y',
    'acc1z',
    'gyro1x',
    'gyro1y',
    'gyro1z',
    'acc2x',
    'acc2y',
    'acc2z',
    'gyro2x',
    'gyro2y',
    'gyro2z',
])


def read_sample_frames(f):
    """Reads sample frames from the file-like object `f`.

    For each sample frame, yields a list of Samples.
    """
    samples = []
    for line in f:
        if line.startswith('#'):
            if len(samples):
                yield samples
                samples = []
        else:
            samples.append(parse_sample(line))
    if len(samples):
        yield samples


def parse_sample(line):
    """Parses a sample line, returning its Sample object"""
    comp = (x.strip() for x in line.split(','))
    return Sample(acc1x=int(next(comp)),
                  acc1y=int(next(comp)),
                  acc1z=int(next(comp)),
                  gyro1x=int(next(comp)),
                  gyro1y=int(next(comp)),
                  gyro1z=int(next(comp)),
                  acc2x=int(next(comp)),
                  acc2y=int(next(comp)),
                  acc2z=int(next(comp)),
                  gyro2x=int(next(comp)),
                  gyro2y=int(next(comp)),
                  gyro2z=int(next(comp)))


def main(args, prog=None):
    p = argparse.ArgumentParser(prog=prog)
    p.add_argument('-o', metavar='OUTPUT', dest='output',
                   type=argparse.FileType('a'), default=sys.stdout,
                   help='output result stream (default: stdout)')
    p.add_argument('input', nargs='?', type=argparse.FileType('r'),
                   default=sys.stdin,
                   help='input sample frame stream (default: stdin)')
    args = p.parse_args(args)
    for sample_frame in read_sample_frames(args.input):
        time.sleep(1)  # Do some processing
        print('wavehands', file=args.output)  # Print result


if __name__ == '__main__':
    main(sys.argv[1:], sys.argv[0])
