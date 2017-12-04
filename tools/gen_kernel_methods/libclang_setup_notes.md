How to set up libclang and Python bindings
==========================================

This worked for me on Ubuntu 16.04 in December of 2017.

Good news. You do not need to build libclang from source. But you will need to
grab the source to get the Python bindings. Here's how to do it.

First, make sure you have deb-src activated in your /etc/apt/sources.list file.
It looks like they are commented out by default (to speed up package list
updates I presume).

Install libclang development library:

    $ sudo apt-get install libclang-4.0-dev

libclang library files will be at /usr/lib/clang/4.0/lib/linux/.

Now download the source to get the Python bindings. This command will download
the source in the current working directory.

    $ apt-get source libclang-4.0-dev

Copy out the Python bindings and get rid of everything else:

    $ cp -r llvm-toolchain-4.0-4.0/clang/bindings/python pyclang
    $ rm -rf llvm-toolchain-*

Make sure you add the pyclang directory to your `PYTHONPATH` so Python knows
where to find the libs when you do an `import`. Do this either via the
`PYTHONPATH` environment variable or doing `sys.path.append()` in your code
before your `import` statement.

You can try out some of the example code that ships with the bindings like so:

    $ PYTHONPATH="pyclang" python pyclang/examples/cindex/cindex-dump.py pyclang/tests/cindex/INPUTS/hello.cpp
