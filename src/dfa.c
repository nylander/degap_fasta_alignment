#include "dfa.h"

void printUsage(const char *program_name) {

    fprintf(stderr, "\nRemove columns in fasta formatted alignments containing all missing data (%s).\n\n", DEFAULT_MISSING_CHARS);
    fprintf(stderr, "Usage:\n\n %s [-h][-v][-V] infile(s).\n\n", program_name);
    fprintf(stderr, "  -h                show help\n");
    fprintf(stderr, "  -V                print version\n");
    fprintf(stderr, "  -v                be verbose\n");
    fprintf(stderr, "  -m missing_chars  list of chars treated as missing\n");
    fprintf(stderr, "  -w wrap_length    wrap seq to length (default: %i)\n\n", DEFAULT_WRAP_LENGTH);
    fprintf(stderr, "  infile should be in fasta format.\n\n");
    fprintf(stderr, "Example:\n\n");
    fprintf(stderr, "  %s -m - in.fas > out.fas\n\n", program_name);

}

void printVersion() {

    fprintf(stdout, "%s\n", VERSION_STR);

}

long int getSeqLength(const char *file_name) {

    char r;
    int inheader = 0;
    long int nseqs = 0;
    long int seqlen = 0;

    gzFile zfp = gzopen(file_name, "rb");
    if (zfp == NULL) {
        perror("Error: failed in opening file");
        exit(EXIT_FAILURE);
    }

    while ((r = gzgetc(zfp)) != EOF) {
        if (inheader) {
            if (r == '\n') {
                inheader = 0;
            }
        }
        else if (r == '>') {
            inheader = 1;
            ++nseqs;
            if (nseqs > 1) {
                break;
            }
        }
        else {
            if (!isspace(r)) {
                ++seqlen;
            }
        }
    }

    gzclose(zfp);

    return seqlen;

}

void *createBitArray(long int n) {

    unsigned char *bit_array = (unsigned char *)malloc(n / 8 + 1);
    if (bit_array == NULL) {
        fprintf(stderr, "Error: Could not allocate memory for bit array.\n");
        exit(EXIT_FAILURE);
    }
    memset(bit_array, 0, n / 8 + 1);

    return (void *)bit_array;

}

void initateBitArray(const char *file_name, bit_array_t *bit_array, const char *missing_chars) {

    char r;
    int inheader = 0;
    long int i = 0;

    gzFile zfp = gzopen(file_name, "rb");
    if (zfp == NULL) {
        perror("Error: failed in opening file");
        exit(EXIT_FAILURE);
    }

    while ((r = gzgetc(zfp)) != EOF) {
        if (inheader) {
            if (r == '\n') {
                inheader = 0;
                i = 0;
            }
        }
        else if (r == '>') {
            inheader = 1;
        }
        else {
            if (!isspace(r)) {
                if (!strchr(missing_chars, r)) {
                    bit_array[i / 8] |= 1 << (i % 8);
                }
                ++i;
            }
        }
    }

    gzclose(zfp);

}

// void printBitArray(bit_array_t *bit_array, long int n) {
//
//     for (long int i = 0; i < n; i++) {
//         if (i % 8 == 0) {
//             fprintf(stderr, "\n");
//         }
//         fprintf(stderr, "%d", (bit_array[i / 8] >> (i % 8)) & 1);
//     }
//     fprintf(stderr, "\n");
//
// }

void printFasta(const char *file_name, bit_array_t *bit_array, int wrap_length) {

    char r;
    int inheader = 0;
    long int i = 0;
    int j = 0;
    int done_first = 0;

    gzFile zfp = gzopen(file_name, "rb");

    if (zfp == NULL) {
        perror("Error: failed in opening file");
        exit(EXIT_FAILURE);
    }

    while ((r = gzgetc(zfp)) != EOF) {
        if (inheader) {
            if (r == '\n') {
                inheader = 0;
                i = 0;
                j = 0;
                done_first = 1;
            }
            fprintf(stdout, "%c", r);
        }
        else if (r == '>') {
            inheader = 1;
            if (done_first) {
                fprintf(stdout, "\n");
            }
            fprintf(stdout, "%c", r);
        }
        else {
            if (!isspace(r)) {
                if ((bit_array[i / 8] >> (i % 8)) & 1) {
                    if (j == wrap_length) {
                       fprintf(stdout, "\n");
                       j = 0;
                    }
                    fprintf(stdout, "%c", r);
                    ++j;
                }
                ++i;
            }
        }
    }

    fprintf(stdout, "\n");

    gzclose(zfp);

}

void freeBitArray(bit_array_t *bit_array) {

    free(bit_array);

}

int main(int argc, char **argv) {

    char *fname;
    int c, err = 0;
    int verbose = 0;
    long int seqlen;
    long int bit_array_size;
    bit_array_t *bit_array;
    char *missing_chars = DEFAULT_MISSING_CHARS;
    int wrap_length = DEFAULT_WRAP_LENGTH;

    if (argc == 1) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((c = getopt(argc, argv, "hVvw:m:")) != -1) {
        switch (c) {
            case 'h':
                printUsage(argv[0]);
                exit(EXIT_SUCCESS);
            case 'V':
                printVersion();
                exit(EXIT_SUCCESS);
            case 'v':
                verbose = 1;
                break;
            case 'w':
                wrap_length = atoi(optarg);
                break;
            case 'm':
                missing_chars = optarg;
                break;
            case '?':
                err = 1;
                break;
        }
    }

    if (err) {
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (optind < argc) {
        for (; optind < argc; optind++) {
            fname = argv[optind];

            if (verbose) {
                fprintf(stderr, "=1= getting sequence length: ");
            }
            seqlen = getSeqLength(fname);
            if (verbose) {
                fprintf(stderr, "%ld bp\n", seqlen);
            }

            bit_array = createBitArray(seqlen);
            bit_array_size = (seqlen / 8 + 1);
            if (verbose) {
                fprintf(stderr, "=2= creating bit array: %ld KB\n", bit_array_size / 1024);
            }
            initateBitArray(fname, bit_array, missing_chars);

            if (verbose) {
                fprintf(stderr, "=3= printing fasta:\n");
            }
            printFasta(fname, bit_array, wrap_length);

            if (verbose) {
                fprintf(stderr, "\n=4= done.\n");
            }

            freeBitArray(bit_array);
        }
    }
    else {
        fprintf(stderr, "Error: Need input fasta file(s) to process.\n");
        printUsage(argv[0]);
        exit(EXIT_FAILURE);
    }

    return EXIT_SUCCESS;

}
