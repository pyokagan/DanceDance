#!/usr/bin/python3
"""Raspi machine learning process"""
import sys
import argparse
import collections
import time
from machine_learning import predict

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


def read_sample_windows(f):
    """Reads sample windows from the file-like object `f`.

    For each sample window, yields a list of Samples.
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
    try:
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
    except StopIteration:
        raise ValueError('not enough elements: ' + line)


def main(args, prog=None):
    p = argparse.ArgumentParser(prog=prog)
    p.add_argument('-o', metavar='OUTPUT', dest='output',
                   type=argparse.FileType('a'), default=sys.stdout,
                   help='output result stream (default: stdout)')
    p.add_argument('input', nargs='?', type=argparse.FileType('r'),
                   default=sys.stdin,
                   help='input sample window stream (default: stdin)')
    args = p.parse_args(args)
    while True:
        result = predict.predict_segment(args.input) 
        if result == 'neutral':
            continue
        time.sleep(1)
        print(result, file=args.output)
        args.output.flush()


if __name__ == '__main__':
    main(sys.argv[1:], sys.argv[0])
