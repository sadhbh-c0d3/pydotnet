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

from dotnet import PyDotnet as _dotnet
import dotnet.classhelp


__all__ = ['isinstance', 'issubclass', 'type', 'help']


try:
    import builtins as _builtin
except:
    import __builtin__ as _builtin


_type = _builtin.type
_isinstance = _builtin.isinstance
_issubclass = _builtin.issubclass
_help = _builtin.help


def __type_imp(x):
    """Tell RuntimeType of the .NET object or class."""
    if hasattr(x, '__typeid__'):
        return x.__typeid__
    raise NotImplementedError


def __isinstance_imp(x, T):
    """Tell if instance 'x' is can be assigned to variable of type 'T'."""
    if not _isinstance(x, _dotnet.Interop.ObjectBase):
        raise NotImplementedError

    if not _isinstance(T, _dotnet.Interop.Type):
        if hasattr(T, '__classid__'):
            T = T.__classid__
        if not _isinstance(T, _dotnet.Interop.Type):
            raise NotImplementedError

    return _dotnet.IsInstance(x, T)


def __issubclass_imp(x, T):
    """Tell if an instance of type 'x' can be helt in variable of type 'T'."""
    if not _isinstance(x, _dotnet.Interop.Type):
        if hasattr(x, '__classid__'):
            x = x.__classid__
        if not _isinstance(x, _dotnet.Interop.Type):
            raise NotImplementedError

    if not _isinstance(T, _dotnet.Interop.Type):
        if hasattr(T, '__classid__'):
            T = T.__classid__
        if not _isinstance(T, _dotnet.Interop.Type):
            raise NotImplementedError

    return T.__typeid__.IsAssignableFrom(x.__typeid__)


def __help_imp(x):
    """Get help for .NET object, type, method, or namespace."""
    return dotnet.classhelp.get_help(x)


def __type(*args):
    if len(args) == 1:
        return __type_imp(*args)    
    raise NotImplementedError


def __isinstance(*args):
    if len(args) == 2:
        return __isinstance_imp(*args)    
    raise NotImplementedError


def __issubclass(*args):
    if len(args) == 2:
        return __issubclass_imp(*args)    
    raise NotImplementedError


def __help(*args):
    if len(args) == 1:
        __help_imp(*args)    
    else:
        raise NotImplementedError


# noinspection PyShadowingBuiltins
def type(*args):
    try:
        return __type(*args)
    except NotImplementedError:
        return _type(*args)


# noinspection PyShadowingBuiltins
def isinstance(*args):
    try:
        return __isinstance(*args)
    except NotImplementedError:
        return _isinstance(*args)


# noinspection PyShadowingBuiltins
def issubclass(*args):
    try:
        return __issubclass(*args)
    except NotImplementedError:
        return _issubclass(*args)


# noinspection PyShadowingBuiltins
def help(*args):
    try:
        return __help(*args)
    except NotImplementedError:
        return _help(*args)


def install():
    import __main__
    __main__.type = type
    __main__.isinstance = isinstance
    __main__.issubclass = issubclass
    __main__.help = help
    import dotnet.commontypes
    for x in dir(dotnet.commontypes):
        if not x.startswith('_'):
            setattr(__main__, x, getattr(dotnet.commontypes, x))

