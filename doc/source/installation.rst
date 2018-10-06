Installation
============

The package is built and installated from source as a CMake project:

- If you have cloned the project from GitHub, run ``./bootstrap.sh`` first.
- Then the normal configuration: ``mkdir build; cd build; cmake ..``.
  Add ``-DCMAKE_INSTALL_PREFIX=<prefix>`` to install in a non-standard location.
  Use ``cmake .. -LH`` to see other options, or see below.
- Build, e.g., ``make -j 8``
- Install, e.g., ``make install``

The dependencies are:

- This documentation requires `Sphinx <http://sphinx-doc.org>`__,
  preferably in the newest development version (use ``-DBUILD_DOC=on``).
- A C++ compiler with reasonable C++14 support is needed. GCC 5.1 or later should work.
- `Boost <http://boost.org>`__ dev >= 1.64
  (use ``-DBOOST_ROOT=<path>`` for non-standard locations).
