# DFA - Degap Fasta Alignments

- Last modified: tis feb 25, 2025  06:12
- Sign: JN

## Description

Remove columns in large fasta-formatted alignments containing all-missing data.

Designed to make minimal memory footprint.

## Installation

See [INSTALL](INSTALL)

## Usage

    $ dfa [-h][-v][-V] infile(s).

## Options:

- `-h`                show help
- `-V`                print version
- `-v`                be verbose
- `-m missing_chars`  list of chars treated as missing.
                      Default characters are `Nn?Xx-`

`infile` should be in fasta format.

## Examples

Input example ([in.fas](data/in.fas))

    $ cat data/in.fas
    >Apa
    ANN-C
    >Bpa
    ANT-C

    $ dfa data/in.fas
    >Apa
    ANC
    >Bpa
    ATC

    $ dfa -m - data/in.fas
    >Apa
    ANNC
    >Bpa
    ANTC

## Scripts

### [`remove_empty_alignment_columns.py`](scripts/remove_empty_alignment_columns.py)

Remove columns from multiple sequence alignment containing all missing data.
Written in python with low memory footprint in mind.

See output from `remove_empty_alignment_columns.py -h` for usage.

### [`degap_fasta_alignment.pl`](scripts/degap_fasta_alignment.pl)

Script written i perl for removing gaps (missing) characters in fasta
alignments. The original script is taken from [fastagap
repository](https://github.com/nylander/fastagap/blob/main/degap_fasta_alignment.pl).
This script have some extra options for removing gaps (missing data).  Note,
however, the script can not handle very large alignments.

See output from `degap_fasta_alignment.pl -h` for usage.

## License and copyright

Copyright 2025 Johan Nylander

[MIT License](LICENSE)

