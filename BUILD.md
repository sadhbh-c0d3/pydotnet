Introduction
============

This is Python for .NET.

See presentation slides: ``pydotnet/slides/Python_vs_dotNET_Interoperability.slides.html``

---------------------------

Module Contents
===============

The *PyDotnet* is project name, while python module name is ``dotnet``.

The *PyDotnet* project was initially purely C++ project, and as such you will see it has typical C++ project structure:

* ``PyDotnet.vcproj`` in root folder
* ``src/*.cpp`` are C++ sources
* ``include/*.h`` are C++ headers

The project builds ``PyDotnet.pyd`` module. This module it-self is fully functional, but is not really nice to the end user. And for that reason ``dotnet`` Python package was created. 

* ``dotnet/*.py`` are source of Python sugar layer, and as you may expect it depends on ``PyDotnet.pyd`` module, so it has to be built first.

The ``PyDotnet.pyd`` module depends on ``Boost.Python`` and ``Boost.RegEx``. 

* ``libs/dotnet-dev/bld.bat`` script downloads Boost v1.60 and using ``python.exe`` pointed by ``%PYTHON%`` environment variable it builds ``Boost.Python`` and ``Boost.RegEx``, and copies into ``Libs`` directory of Python whose ``python.exe`` is pointed by ``%PYTHON%`` environment variable.

The ``libs/dotnet-dev/bld.bat`` has to be ran before building ``PyDotnet.pyd`` and must be ran from ``libs/dotnet-dev`` directory.

---------------------------

Build System
============

Building process is very very easy. All you need is Python and Visual Studio.

Follow this README and run commands as shown below.

* We recommend using Anaconda, but Python is also good.

* We recommend Visual Studio 2015, but Visual Studio 2012 should also work.

* Visual Studio 2015 Community Edition can be downloaded for free from: 
    https://www.visualstudio.com/products/visual-studio-community-vs

* You need C++ compiler and .NET framework installed.

---------------------------

Building Step-by-step
=====================

Step 1. Dependencies
--------------------

In order to build ``PyDotnet.pyd`` first you need to build ``Boost.Python`` and ``Boost.Regex``.

Run following commands:

    cd libs/dotnet-dev

    set PYTHON="C:/Python/python.exe"

    bld.bat

If you are using *Anaconda* you may try instead these:

    cd libs

    conda build dotnet-dev`

    conda install dotnet-dev

However I know there is some issue with that approach.

The ``bld.bat`` script in ``libs/dotnet-dev`` **downloads, builds and installs** dependencies from source using Visual Studio (auto-detected: 2015 has priority over 2012 if you had both) against Python (version is auto-detected: 2.7+ or 3.+).

The ``PYTHON`` environment variable controls which Python installation to use. We assume to build all package versions you need multiple versions of Python installed, in order to get package for each of those versions.



Step 2. Module
--------------

Once you build dependencies in Step 1, now you can build ``PyDotnet.pyd`` module.

You do this by running command:

    ``python setup.py build``

and then to install

     ``python setup.py install``


Development
===========

Step 1. Build in-place module
-----------------------------

The ``dotnet`` Python module can be developed on the go, i.e. you can edit ``dotnet/*.py`` files and launch python and ``import dotnet`` will load new edited version of ``.py`` sources.

To avail of this you need to build module in place, and you can do that by running command:

    ``python setup.py develop``


Step 2. Build and debug with Visual Studio
------------------------------------------

The ``PyDotnet.pyd`` can be built and debugged from within Visual Studio.

Run in command line:

    ``for /f %p in ('python -c "import sys; print(sys.prefix)"') do set`` PREFIX=%p

    ``setx PREFIX %PREFIX%``

Next launch Visual Studio, either click icon or run:

    ``C:/VS14/Common7/IDE/devenv.exe``

Then Visual Studio will look for C++ includes and libs in ``%PREFIX%/Library/include``, ``%PREFIX%/Library/lib``, ``%PREFIX%/libs and %PREFIX%/include``, where ``PREFIX`` environment variable points to Python folder.


Authors
=======

* Sonia Kolasinska <sonia.kolasinska.pro@gmail.com>
* Ivan Smirnov
* Ivan Kalev <ivan.kalev@gmail.com>
* Jack Higgins
