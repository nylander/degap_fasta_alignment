AUTOMAKE_OPTIONS = subdir-objects
bin_PROGRAMS = dfa
dfa_SOURCES = src/dfa.c src/dfa.h
dfa_CFLAGS = -O3 -march=native -flto -funroll-loops -finline-functions
dfa_LDADD = -lz
