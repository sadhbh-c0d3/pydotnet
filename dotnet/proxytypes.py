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


Tuple1 = _dotnet.GlobalNamespace['System.Tuple`1']
Tuple2 = _dotnet.GlobalNamespace['System.Tuple`2']
Tuple3 = _dotnet.GlobalNamespace['System.Tuple`3']
Tuple4 = _dotnet.GlobalNamespace['System.Tuple`4']
Tuple5 = _dotnet.GlobalNamespace['System.Tuple`5']
Tuple6 = _dotnet.GlobalNamespace['System.Tuple`6']
Tuple7 = _dotnet.GlobalNamespace['System.Tuple`7']
Tuple8 = _dotnet.GlobalNamespace['System.Tuple`8']


ImplicitConversion = _dotnet.GlobalNamespace['InteropPython.ImplicitConversion`1']
ArrayHelper = _dotnet.GlobalNamespace['InteropPython.ArrayHelper`1']


class ProxyType(object):
    """Proxy to .NET class."""
    """Any .NET class is normally represented with _dotnet.Interop.Type."""
    """However it cannot be extended in Python, so we have this ProxyType to solve this issue."""
    """The __class__ property shall point to _dotnet.Interop.Type representing final .NET type."""
    """The purpose of ProxyType is to provide meta-class, which will calculate final .NET type"""
    """based on some parameters. For example .NET does not define generic Array<T> type, but in Python"""
    """we'd like to create array types using Array[T](...) ctor. We would also like Array[T] to"""
    """behave as _dotnet.Interop.Type, e.g. List[Array[Int32]] shall be evaulated to List<Int32[]>."""
    """Another example would be Func<.> and Action<.>, which don't have any ctor, but can be created"""
    """from lambda expression or a method. That is an implicit type conversion, i.e. we want to covert"""
    """Python type into some .NET type implicitly in order to create an instance of that .NET type."""
    """Implicit conversion is handled whenever you call .NET function from Python, but """
    """you need a function to call, and an instance of .NET object disappears after the call."""
    """To solve this we have ImplicitConversion, which returns into Python the instance that """
    """was implicitly created. A ProxyType can be used to facilitate this, e.g. Func[Int32] will"""
    """evaluate to Func<Int32>, and Func[String, Int32] to Func<String, Int32>. Also we may want"""
    """to create an instance of .NET object from some Python native type, e.g. {} (dict type) can"""
    """be used to create System.Collections.Generic.Dictionary<.>, but only via implicit conversion."""
    @property
    def __typeid__(self):
        """Returns runtime type information."""
        """Default implementation depends on __class__ being implemented."""
        """Failure to implement __class__ property will result in infinite recursion."""
        return self.__class__.__typeid__

    @property
    def __class__(self):
        """Returns class object for type"""
        """Default implementation depends on __typeid__ being implemented."""
        return _dotnet.Interop.Type(self.__typeid__)

    def __call__(self, *args):
        """Creates an instance of the object"""
        """Default implementation depends on __createinstance__ being implemented."""
        return self.__createinstance__(*args)

    def __str__(self):
        return str(self.__class__)

    def __repr__(self):
        return repr(self.__class__)

    def __pretty__(self):
        return repr(self.__class__)

    def __getattr__(self, name):
        return getattr(self.__class__, name)

    @property
    def __help__(self):
        return self.__class__.__help__



class ArrayType(ProxyType):
    """Array cannot be constructed in Python, but"""
    """it can be created by implicit type conversion"""
    def __init__(self, T):
        super(ArrayType, self).__init__()
        self.T = T

    def __createinstance__(self, arg = None):
        if not arg:
            return ArrayHelper[self.T].CreateArray(0)
        elif isinstance(arg, int):
            return ArrayHelper[self.T].CreateArray(arg)
        else:
            return ArrayHelper[self.T].GetArray(arg)

    @property
    def __typeid__(self):
        return ArrayHelper[self.T].GetArrayType()


class GenericArray(ProxyType):
    """Array cannot be constructed in Python, but"""
    """it can be created by implicit type conversion"""
    def __getitem__(self, T):
        return ArrayType(T)

    def __call__(self):
        raise Exception('Unspecialized generic type')

    @property
    def __class__(self):
        """Returns class object for type"""
        return _dotnet.GlobalNamespace['System.Array']


Array = GenericArray()


class ImplicitType(ProxyType):
    """The type, the instance of which cannot be constructed in Python, but"""
    """it can be created by implicit type conversion."""
    def __init__(self, T):
        super(ImplicitType, self).__init__()
        self.T = T

    def __createinstance__(self, x = None):
        if x is None:
            return self.T()
        else:
            return ImplicitConversion[self.T].Convert(x)

    @property
    def __class__(self):
        """Returns class object for type"""
        return self.T


class GenericImplicitType(ProxyType):
    """A generic type, the instance of which cannot be constructed in Python, but"""
    """it can be created by implicit type conversion."""
    def __init__(self, T):
        super(GenericImplicitType, self).__init__()
        self.T = T

    def __getitem__(self, *T):
        return ImplicitType(self.T.__getitem__(*T))

    def __call__(self):
        raise Exception('Unspecialized generic type')

    @property
    def __class__(self):
        """Returns class object for type of unspecialized generic type"""
        return self.T


# Note: Action0 is special as it is non-generic type
Action0 = ImplicitType(_dotnet.Interop.Type(_dotnet.GlobalNamespace['System.Type'].GetType('System.Action')))

Action1 = GenericImplicitType(_dotnet.GlobalNamespace['System.Action`1'])
Action2 = GenericImplicitType(_dotnet.GlobalNamespace['System.Action`2'])
Action3 = GenericImplicitType(_dotnet.GlobalNamespace['System.Action`3'])
Action4 = GenericImplicitType(_dotnet.GlobalNamespace['System.Action`4'])
Action5 = GenericImplicitType(_dotnet.GlobalNamespace['System.Action`5'])
Action6 = GenericImplicitType(_dotnet.GlobalNamespace['System.Action`6'])
Action7 = GenericImplicitType(_dotnet.GlobalNamespace['System.Action`7'])
Action8 = GenericImplicitType(_dotnet.GlobalNamespace['System.Action`8'])

Func1 = GenericImplicitType(_dotnet.GlobalNamespace['System.Func`1'])
Func2 = GenericImplicitType(_dotnet.GlobalNamespace['System.Func`2'])
Func3 = GenericImplicitType(_dotnet.GlobalNamespace['System.Func`3'])
Func4 = GenericImplicitType(_dotnet.GlobalNamespace['System.Func`4'])
Func5 = GenericImplicitType(_dotnet.GlobalNamespace['System.Func`5'])
Func6 = GenericImplicitType(_dotnet.GlobalNamespace['System.Func`6'])
Func7 = GenericImplicitType(_dotnet.GlobalNamespace['System.Func`7'])
Func8 = GenericImplicitType(_dotnet.GlobalNamespace['System.Func`8'])

Dictionary = GenericImplicitType(_dotnet.GlobalNamespace['System.Collections.Generic.Dictionary`2'])

__all__ = list(map(lambda i: 'Action%s' % i, range(0,9))) + \
        list(map(lambda i: 'Func%s' % i, range(1,9))) + \
        list(map(lambda i: 'Tuple%s' % i, range(1,9))) + \
        ['Array', 'Dictionary']

