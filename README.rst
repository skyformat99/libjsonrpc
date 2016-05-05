libjsonrpc
==========

.. image:: https://img.shields.io/badge/license-mit-blue.svg

``libjsonrpc`` is a minimal C implementation of a JSON-RPC v2.0 client library.

Dependencies
------------

``libjsonrpc`` depends on ``curl`` and ``jansson``, so those must be installed on the system.

Building and Installing
-----------------------

This project uses standard GNU Make, so it's easy to build and install.

::

    git clone https://github.com/zmingee/libjsonrpc.git
    cd libjsonrpc
    make

*Note :: Make install isn't actually implemented yet, so you'll have to manually "install" it for
now*

Running Tests
-------------

The testing framework is built on Minunit, and uses the MIT License (just like this project). Big
thanks to David Siñuela Pastor for building Minunit.

Running the tests is simple:

::

    make check
