# Release Build

You need is Python and Visual Studio (see [Releases](https://github.com/sadhbh-c0d3/pydotnet/releases) for versions supported).
Follow this README and run commands as shown below.

* Install [Python for Windows (AMD64)](https://www.python.org/downloads/windows/) 
* Install [Visual Studio Community Edition](https://www.visualstudio.com/products/visual-studio-community-vs)
    * Select
        * C++ compiler
        * .NET framework

## Select Python

Set `%PYTHON%` environment variable:

```
    set PYTHON="path\to\your\python.exe"
```

**Note** *This step is optional, and if you don't set `%PYTHON%` environment variable, then default installation path of Python will be used.
If you prefer, you can use [virtualenv](https://pypi.org/project/virtualenv/) instead.*


## Build Dependencies

Now you need to build `libs-dotnet-dev`:

```
    cd libs/dotnet-dev
    bld.bat
```

This will download `Boost` library, and it will build `Boost.Python` for Python version currently selected by `%PYTHON%` environment variable.

## Build Module

```
    python setup.py build
```

## Install Module

```
    python setup.py install
```


# Developent

## Develop Python code using in-place module

Module can be build in-place so that you can edit ``dotnet/*.py`` files and launch python and ``import dotnet`` will load new edited version of ``.py`` sources.

```
    python setup.py develop
```

## Debug C++ code in Visual Studio

The ``PyDotnet.pyd`` can be built and debugged from within Visual Studio.

Run in command line:

```
    for /f %p in ('python -c "import sys; print(sys.prefix)"') do set PREFIX=%p
    setx PREFIX %PREFIX%
```

Next launch Visual Studio.

Then Visual Studio will look for C++ includes and libs in ``%PREFIX%/Library/include``, ``%PREFIX%/Library/lib``, ``%PREFIX%/libs and %PREFIX%/include``, where ``PREFIX`` environment variable points to Python folder.


Authors
=======

* Sonia Kolasinska <sonia.kolasinska.pro@gmail.com>
* Ivan Smirnov
* Ivan Kalev <ivan.kalev@gmail.com>
* Jack Higgins
