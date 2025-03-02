#! /usr/bin/env python3
# vim:fenc=utf-8
#
# Copyright © 2025 nylander <johan.nylander@nrm.se>
# Distributed under terms of the MIT license.
# Last modified: sön mar 02, 2025  05:49
# Sign: JN

"""
Remove columns from multiple sequence alignment containing all missing data (letters N or - or ?).

Optimized for low memory footprint on large data sets (tested on 94 x 368,485,006 bp data)

"""

import sys
import argparse
from bitarray import bitarray

wrap_default = 60
VERSION = "1.1"

def get_seq_len(fasta_file):
    """
    Read fasta file and return length of first sequence
    """
    seq_len = 0
    nseqs = 0
    with open(fasta_file, encoding="utf-8") as infile:
        for line in infile:
            if line.startswith('>'):
                nseqs += 1
                if nseqs > 1:
                    break
            elif nseqs == 1:
                seq_len += len(line.strip())
    return seq_len

def init_bit_array(fasta_file, missing, bit_array):
    """
    Read fasta file and initiate bit array with True for all columns with non-missing data
    """
    i = 0
    with open(fasta_file, encoding="utf-8") as infile:
        for line in infile:
            if line.startswith('>'):
                i = 0
            else:
                for char in line.strip():
                    if char not in missing:
                        bit_array[i] = True
                    i += 1

def print_fasta(fasta_file, bit_array, wrap):
    """
    Read fasta file and print all columns with non-missing data
    """
    i = 0
    j = 0
    done_first = 0
    with open(fasta_file, encoding="utf-8") as infile:
        for line in infile:
            # strip line from newline character
            line = line.strip()
            if line.startswith('>'):
                if done_first:
                    print('', file=sys.stdout)
                print(line, end='\n', file=sys.stdout)
                i = 0
                j = 0
                done_first = 1
            else:
                for char in line.strip():
                    if char != '\n':
                        if bit_array[i]:
                            j = j + 1
                            print(char, end='', file=sys.stdout)
                            if j == wrap:
                                print("\n", end='', file=sys.stdout)
                                j = 0
                        i += 1
        print('', file=sys.stdout)

def parse_args():
    """
    Parse command line arguments
    """
    parser = argparse.ArgumentParser(
        description='Remove columns from multiple sequence alignment containing all missing data')
    parser.add_argument('fasta_file',
                        help='Input fasta file')
    parser.add_argument('-m', '--missing', nargs='+', default=['N', '-', '?', 'X', 'n', 'x'],
                        help='Missing data characters')
    parser.add_argument('-w', '--wrap', nargs='+', default=wrap_default,
                        help='Set line line length for sequence in output.')
    parser.add_argument('-V', '--version', action='version', version='%(prog)s ' + VERSION,
                        help='Print version and exit')
    parser.add_argument('--verbose', '-v', action='count', default=0,
                        help='Increase verbosity')
    return parser.parse_args()

def main():
    """
    Main function
    """
    args = parse_args()

    if args.verbose > 0:
        print(f"Step 1(3): Getting initial seq length...", file=sys.stderr)

    seq_length = get_seq_len(args.fasta_file)
    if args.verbose > 0:
        print(f"Initial seq length: {seq_length}", file=sys.stderr)

    bit_array = bitarray(seq_length)
    bit_array.setall(False)
    if args.verbose > 2:
        print(f"bit array start: {bit_array}", file=sys.stderr)

    if args.verbose > 0:
        print(f"Step 2(3): Initiating bit array...", file=sys.stderr)

    init_bit_array(args.fasta_file, args.missing, bit_array)
    if args.verbose > 2:
        print(f"bit array end: {bit_array}", file=sys.stderr)

    if args.verbose > 0:
        print(f"Number of columns with missing data: {bit_array.count(False)}", file=sys.stderr)

    if args.verbose > 0:
        print(f"Step 3(3): Printing remaining alignment columns...", file=sys.stderr)

    print_fasta(args.fasta_file, bit_array, args.wrap)

    if args.verbose > 0:
        print(f"End of script.", file=sys.stderr)

if __name__ == '__main__':
    main()
