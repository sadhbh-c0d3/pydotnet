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
from dotnet import PyDotnet as _dotnet
try:
    from functools import reduce
except:
    pass


clr = _dotnet.GlobalNamespace


def __namespaces():
    return _dotnet.GlobalNamespace.Namespaces


def __namespaces_filtered(f):
    if isinstance(f, str):
        return filter(lambda x: f in x, __namespaces())
    elif hasattr(f, '__call__'):
        return filter(f, __namespaces())
    else:
        raise NotImplementedError


def __namespaces_filtered_many(*args):
    for ns in __namespaces():
        if any(x in ns for x in args):
            yield ns


def namespaces(*args):
    """Gets list of loaded namespaces"""
    if len(args) == 0:
        return __namespaces()
    elif len(args) == 1:
        return __namespaces_filtered(*args)
    else:
        return __namespaces_filtered_many(*args)


def __loaded_assemblies():
    return _dotnet.GlobalNamespace.LoadedAssemblies


def __loaded_assemblies_filtered(f):
    if isinstance(f, str):
        return filter(lambda x: f in x.FullName, __loaded_assemblies())
    elif hasattr(f, '__call__'):
        return filter(f, __loaded_assemblies())
    else:
        raise NotImplementedError


def __loaded_assemblies_filtered_many(*args):
    for ns in __loaded_assemblies():
        if any(x in ns for x in args):
            yield ns


def loaded_assemblies(*args):
    """Gets list of loaded assemblies"""
    if len(args) == 0:
        return __loaded_assemblies()
    elif len(args) == 1:
        return __loaded_assemblies_filtered(*args)
    else:
        return __loaded_assemblies_filtered_many(*args)


def load_assembly(nameOrPath):
    """Loads .NET assembly by name or from file path."""
    _dotnet.GlobalNamespace.LoadAssembly(nameOrPath)


def build_assembly(sourceCode, outputFile, assemblies, compilerOptions):
    """Builds .NET assembly from C# source code."""
    _dotnet.GlobalNamespace.LoadSource(sourceCode, outputFile, assemblies, compilerOptions)


def get_namespace(name):
    x = _dotnet.GlobalNamespace[name]
    if not isinstance(x, _dotnet.Interop.Namespace):
        raise TypeError(name + ' is not a namespace')
    return x


def get_class(name, num_generic_type_parameters = 0):
    """Returns class object for type with given name and"""
    """optionally number of generic type parameters."""
    """e.g. type 'Func<int, int>' has a name 'Func`2'."""
    if not num_generic_type_parameters:
        x = _dotnet.GlobalNamespace[name]
        if not isinstance(x, _dotnet.Interop.Type):
            raise TypeError(name + ' is not a type')
        return x
    else:
        return _dotnet.GlobalNamespace[name + '`' + str(num_generic_type_parameters)]


def make_class(x):
    """Returns class object for runtime type information"""
    if not isinstance(x, _dotnet.Interop.Object):
        raise TypeError(name + ' is not a type instance')
    return _dotnet.Interop.Type(x)


def install_getattrhook(hook):
    prev_hook = _dotnet.Interop.Object.__getattrhook__
    if prev_hook is None:
        _dotnet.Interop.Object.__getattrhook__ = hook
    else:
        _dotnet.Interop.Object.__getattrhook__ = lambda obj, name, get: \
            hook(obj, name, lambda obj_, name_: \
                    prev_hook(obj_, name_, get))


def uninstall_getattrhooks():
    _dotnet.Interop.Object.__getattrhook__ = None


#
# TODO: Move below functions to 'dotnet.utils'
#

def pretty(x):
    """Returns pretty representation if available"""
    if hasattr(x, '__pretty__'):
        return x.__pretty__()
    elif isinstance(x, dict):
        return '\n'.join(map(lambda x: '%s: %s' % x, x.items()))
    elif hasattr(x, '__iter__'):
        return '\n'.join(map(pretty, x))
    else:
        return repr(x)


def print_pretty(x):
    """Prints pretty(x)"""
    print(pretty(x))


def _add_many(a,x):
    a.setdefault(x[0], []).append(x[1])
    return a


def _multi_dict(kv_pairs):
    return reduce(_add_many, kv_pairs, {})


def pretty_names(sequence, key_width = 2, max_rowlen = 4):
    kv_pairs = map(lambda x: (x[:key_width], x), sequence)
    d = _multi_dict(kv_pairs)
    lists = [], [], []
    for k, v in sorted(d.iteritems()):
        i = 2 if k[0] is '_' else 0 if k[0] == k[0].upper() else 1
        while len(v) > max_rowlen:
            a,v = v[:max_rowlen], v[max_rowlen:]
            s = ', '.join(a)
            lists[i].append(s)
        s = ', '.join(v)
        lists[i].append(s)
    return '\n\n'.join('\n'.join(x) for x in lists[:2] if not not x)


def print_pretty_names(*args):
    print(pretty_names(*args))

