# The MIT License (MIT) Copyright (c) 2016, Susquehanna International Group
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is furnished to do
# so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

from __future__ import print_function
import dotnet.moduleloader
import dotnet.basics
import dotnet.commontypes
import glob
import os


from System import AppDomain
from System.Reflection import Assembly, AssemblyName


def is_assembly(path):
    try:
        if os.path.isfile(path):
            if AssemblyName.GetAssemblyName(path) is not None:
                return True
    except:
        pass
    return False
    

def find_assemblies(rootdir):
    """Search for assemblies in specified directory"""
    spat = os.path.join(rootdir, r'*.dll')
    fres = glob.glob(spat)
    for fpath in fres:
        # take file-name (no parent-path and no file-extension)
        if is_assembly(fpath):
            mname = os.path.splitext(os.path.basename(fpath))[0]
            yield mname, fpath


class AssemblyResolver(object):
    def __init__(self, arg = None):
        self.__assemblies = {}
        if arg is not None:
            self.add(arg)
    
    def add(self, arg):
        """Add assemblies from path or list."""
        """ @arg: can be either:"""
        """ - str pointing to root directory, or"""
        """ - dict mapping module name to file path, or"""
        """ - seq of module name, file path tuples"""
        if isinstance(arg, str):
            self.__add(dict(find_assemblies(arg)))
        elif isinstance(arg, dict):
            self.__add(arg)
        else:
            self.__add(dict(arg))

    def install(self, domain = None):
        """Install assembly loader in current or selected domain"""
        if domain is None:
            domain = AppDomain.CurrentDomain
        domain.add_AssemblyResolve(self.__load)

    def load_assembly(self, mname):
        fname = self.__assemblies[mname]
        dotnet.basics.load_assembly(fname)

    def __getitem__(self, name):
        """Get assembly file path by module name"""
        return self.__assemblies[name]

    def __len__(self):
        return len(self.__assemblies)

    def __iter__(self):
        return iter(self.__assemblies)

    def __add(self, assembiles):
        self.__assemblies.update(assembiles)

    def __load(sender, args):
        print('Loading', args.Name)
        mname = args.Name.split(',')[0]
        fpath = self.__assemblies[mname]
        return Assembly.LoadFile(fpath)


__resolver = None


def set_assemblies(arg):
    """Set available assemblies."""
    """ @arg: can be either:"""
    """ - str pointing to root directory, or"""
    """ - dict mapping module name to file path, or"""
    """ - seq of module name, file path tuples"""
    global __resolver
    __resolver = AssemblyResolver(arg)


def add_assemblies(arg):
    """Add assemblies to the list of available assemblies."""
    """ @arg: can be either:"""
    """ - str pointing to root directory, or"""
    """ - dict mapping module name to file path, or"""
    """ - seq of module name, file path tuples"""
    global __resolver
    if __resolver is None:
        __resolver = AssemblyResolver()
    __resolver.add(arg)

def add_framework_assemblies():
    """Add .NET framework assemblies to the list of available assemblies."""
    """Not done by default as it may be slow and not always required"""
    path = dotnet.commontypes.framework_path()
    add_assemblies(path)

def load_assembly(nameOrPath):
    """Load assembly using AssemblyResolver."""
    if is_assembly(nameOrPath):
        dotnet.basics.load_assembly(nameOrPath)
    global __resolver
    if __resolver is None:
        raise ValueError('Assembly not found: %s' % nameOrPath)
    __resolver.load_assembly(nameOrPath)


def __assemblies():
    if __resolver is None:
        return iter(())
    return iter(__resolver)


def __assemblies_filtered(f):
    if isinstance(f, str):
        return filter(lambda x: f in x, __assemblies())
    elif hasattr(f, '__call__'):
        return filter(f, __assemblies())
    else:
        raise NotImplementedError


def __assemblies_filtered_many(*args):
    for ns in __assemblies():
        if any(x in ns for x in args):
            yield ns


def assemblies(*args):
    """Get list of available assemblies."""
    if len(args) == 0:
        return __assemblies()
    elif len(args) == 1:
        return __assemblies_filtered(*args)
    else:
        return __assemblies_filtered_many(*args)


def load_assemblies(*args):
    """Load all available assemblies."""
    for asn in assemblies(*args):
        load_assembly(asn)


__all__ = [
        'assemblies', 
        'set_assemblies', 
        'add_assemblies',
        'add_framework_assemblies',
        'load_assembly', 
        'load_assemblies']


