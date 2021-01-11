.. _installation:

Installation
============


From a Git Repository
---------------------

After a checkout of the desired version, do::

	./bootstrap.sh

This is needed to generate build files, extract the API documentation,
and create the file ``VERSION`` based on the current commit.

See :ref:`source-build` on how to then build the package.


As Dependency of Another CMake Project
--------------------------------------

PermGroup supports use via ``add_subdirectory`` in CMake.
The target ``PermGroup::perm_group`` is exported,
which can be used with ``target_link_libraries``.
The version is variable in the variable ``PermGroup_VERSION``.
Note that running ``./bootstrap.sh`` is still needed if the
source is a repository clone (e.g., a Git submodule).


.. _source-build:

From a Source Archive
---------------------

The package is build and installed from source as a CMake project.
Generally that means something like::

	mkdir build
	cd build
	cmake ../ <options>
	make -j <n>
	make install

A source archive can also be created with ``make dist``.

The following is a list of commonly used options for ``cmake``,
and additional options specific for PermGroup.

- ``-DCMAKE_INSTALL_PREFIX=<prefix>``, set a non-standard installation directory.
  Note also that the
  `GNUInstallDirs <https://cmake.org/cmake/help/latest/module/GNUInstallDirs.html>`__
  module is used.
- ``-DCMAKE_BUILD_TYPE=<build type>``, set a non-standard build type.
  The default is `RelWithDebInfo <https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html?highlight=build_type#variable:CMAKE_BUILD_TYPE>`__.
  An additional build type ``OptDebug`` is available which adds the compilation flags ``-g -O3``.
- ``-DBUILD_DOC=on``, whether to build documentation or not.
  This is forced to ``off`` when used via ``add_subdirectory``.
- ``-DBUILD_TESTING=off``, whether to allow test building or not.
  This is forced to ``off`` when used via ``add_subdirectory``.
  When ``on`` the tests can be build with ``make tests`` and run with ``ctest``.
- ``-DBUILD_TESTING_SANITIZERS=on``, whether to compile tests with sanitizers or not.
  This has no effect with code coverage is enabled.
- ``-DBUILD_COVERAGE=off``, whether to compile code and run tests with GCov.
  When ``on`` the sanitizers on tests will be disabled.
  After building the tests, execute ``make coverage_collect`` without parallel jobs to run tests.
  Afterwards, execute ``make coverage_build`` to compile the code coverage report.


Dependencies
------------

- This documentation requires a supported version of `Sphinx <http://sphinx-doc.org>`__
  (``-DBUILD_DOC=on``).
- A C++ compiler with reasonable C++14 support is needed. GCC 5.1 or later should work.
- `Boost <http://boost.org>`__ dev >= 1.64
  (use ``-DBOOST_ROOT=<path>`` for non-standard locations).
- Running tests requires `Sage <http://www.sagemath.org/>`__ (``-DBUILD_TESTING=on``).
- Running tests with code coverage requires GCov, i.e.,
  the commands ``gcov``, ``lcov``, and ``genhtml`` (``-DBUILD_COVERAGE=on``).
