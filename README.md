# mhng: My 4th {,N}MH clone

Copyright 2014-2017 Palmer Dabbelt <palmer@dabbelt.com>

## Getting Started

### Building MHng

To build and install MHng, run the following commands

$ pconfigure
$ make
$ make install

The full list of dependinces, along with an up-to-date build and test
script, can be found in `.travis.yml`.

### Configuring MHng

MHng supports very limited configuration options: see the `mhng-install`
script to generate a new installation in your home directory, which will
then list some configuration options.

### Using MHng

While thit started as an {,N}MH clone, it's changed a lot over the past
few years so it's probably not just going to be a drop-in replacement
for you.  What little documentation that exists can be seen in the
`test/` directory.  Aside from that, it's meant to look a bit like other
MH clones (`scan`, `show`, `repl`).
