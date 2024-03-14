"""
The standard way of building this package is to use pps:

    $ python setup.py conda --target 2.7

and then install it from the "dist" folder.

If you want to build this package in place, you must use the
VS 2012 x64 command prompt:

    $ call %VS110COMNTOOLS%\..\..\VC\vcvarsall x64
    $ conda install --file requirements-build.txt
    $ cd pydotnet
    $ python setup.py build_ext --inplace
    $ set pythonpath=.

"""

summary ="""Python Interoperability with the Microsoft .NET Framework"""

long_description = """Allows seamless integration of .NET (e.g. C#) code with Python (CPython, Anaconda).

It runs as hybrid native code and CLR code. The CLR instance is created once
you import dotnet module into your Python code. Then you can load assemblies
and import namespaces and types directly into Python code, and use as Python
objects. This all runs within Python process, and no IPC is used to accomplish
this. Boost.Python has been used to write C++ layer, which glues Python and
Managed (.NET/CLR/C#) code."""

classifiers = """Development Status :: 5 - Production/Stable
Intended Audience :: Developers
License :: OSI Approved :: MIT License
Topic :: Software Development :: Libraries :: Python Modules
Operating System :: Microsoft :: Windows
Topic :: Software Development
Topic :: Software Development :: Compilers
Topic :: Software Development :: Libraries
Topic :: Software Development :: Quality Assurance
Topic :: System"""

import os
import sys
import glob
import shutil

from setuptools import setup, find_packages
from setuptools.extension import Extension

from build import Command, VersionResolver


version = VersionResolver.find_version()
deps = open("requirements.txt").read().splitlines()

ext = Extension(
    'dotnet.PyDotnet',
    sources=glob.glob("src/*.cpp"),
    include_dirs=["include",
                  os.path.join(sys.prefix, "include"),
                  os.path.join(sys.prefix, "Library", "include")],
    library_dirs=[os.path.join(sys.prefix, "Library", "lib"),
                  os.path.join(sys.prefix, "libs")],
    extra_compile_args=['/MD', '/EHa', '/clr', '/D_SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS'],
    language="c++"
)


def force_vs():
    """
    We don't to build this extension with the usual toolset (e.g. VS 2022
    for Python 2.7). We want to force distutils to pick whatever is in our
    PATH, which must be VS 2022 or newer. We can't build the extension with
    the "correct", python-specific toolset, because we're targeting a newer
    .NET framework.
    """
    Command.use_toolset(r"C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall")


def deploy_boost():
    """
    Deploy all boost runtime dependencies side by side with the C++
    extension.
    """
    lib = os.path.join(sys.prefix, "Library", "lib")
    dlls = os.path.join(lib, "boost_python*.dll")

    for dll in glob.glob(dlls):
        shutil.copy(dll, "dotnet")


def main():

    force_vs()
    deploy_boost()

    setup(
        name=version.package,
        version=version.short,
        url="https://github.com/sadhbh-c0d3/pydotnet",
        author="Sonia Kolasinska",
        author_email="sonia.kolasinska.pro@gmail.com",
        maintainer="Sonia Kolasinska",
        maintainer_email="sonia.kolasinska.pro@gmail.com",
        license = "MIT",
        platforms = ["Windows (x86 and x64)"],
        description = summary,
        long_description  = long_description,
        classifiers = classifiers.split("\n"),
        packages=find_packages(),
        include_package_data=True,
        ext_modules=[ext],
        install_requires=deps,
        options={
            'build_ext': {'compiler': "msvc"}
        },
        cmdclass=Command.find_commands()
    )


if __name__ == "__main__":
    main()
