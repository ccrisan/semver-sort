# About

A command-line tool written in C to sort [semver](https://semver.org/) versions.

Originally cloned from https://github.com/h2non/semver.c, with the addition of `semver-sort.c`.

# Usage:

    <versions> | semver-sort [options]

# Available options:

    -h         show this help
    -k <num>   extract and use field <num> instead of all line (starts at 1)
    -r         sort descending
    -t <char>  use <char> as field separator (defaults to space)
    -q         do not print any messages
    -v         print only versions, not entire lines
