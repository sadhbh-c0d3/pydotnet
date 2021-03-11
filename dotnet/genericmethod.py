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
from dotnet.basics import clr, install_getattrhook
from dotnet.overrides import type
from dotnet.proxytypes import Array


g_DebugGenericMethod = False


def select_type(pytyp):
    if pytyp is int:
        return clr.System.Int32
    if pytyp is long:
        return clr.System.Int64
    if pytyp is float:
        return clr.System.Double
    if pytyp is str:
        return clr.System.String
    if pytyp is list:
        return clr.System.Collections.IList
    if pytyp is dict:
        return clr.System.Collections.IDictionary
    return pytyp


def contains_generic_overloads(f):
    fi = f.__func__
    if isinstance(fi, list):
        return any(x.ContainsGenericParameters for x in fi)
    else:
        return fi.ContainsGenericParameters


def find_method_overload_for_args(gf, *args):
    if g_DebugGenericMethod:
        print('find_method_overload_for_args', gf, args)
    if isinstance(gf.__func__, list):
        for f in gf.__func__:
            if len(f.GetParameters()) == len(args):
                return f, gf.__instance__
    return gf.__func__, gf.__instance__


def find_method_overload_for_types(gf, *args):
    if g_DebugGenericMethod:
        print('find_method_overload_for_types', gf, args)
    if isinstance(gf.__func__, list):
        for f in gf.__func__:
            if len(f.GetGenericArguments()) == len(args):
                return f, gf.__instance__
    return gf.__func__, gf.__instance__


def find_method_generic_args(f, *args):
    if g_DebugGenericMethod:
        print('find_method_generic_args', f, args)
    sa = []
    for i,g in enumerate(f.GetGenericArguments()):
        match = (j for j,p in enumerate(f.GetParameters()) if p.ParameterType == g)
        try:
            j = next(iter(match))
            t = select_type(type(args[j]))
            sa.append(t)
        except StopIteration:
            pass
    return sa


def spec_generic_method(f, generic_args, instance):
    if g_DebugGenericMethod:
       print('spec_generic_method', f, generic_args, instance)
    if not isinstance(generic_args, list):
        generic_args = list(generic_args)
    spec_types = Array[clr.System.Type](generic_args)
    spec_f = f.MakeGenericMethod(spec_types)
    if instance is None:
       return _dotnet.Interop.Method(spec_f)
    return _dotnet.Interop.Method(spec_f, instance)


def spec_generic_method_for_args(gf, *args):
    if g_DebugGenericMethod:
        print('spec_generic_method_for_args', gf, args)
    f, instance = find_method_overload_for_args(gf, *args)
    if not f.ContainsGenericParameters:
        return gf
    generic_args = find_method_generic_args(f, *args)
    return spec_generic_method(f, generic_args, instance)


def spec_generic_method_for_types(gf, *args):
    if g_DebugGenericMethod:
        print('spec_generic_method_for_types', gf, args)
    f, instance = find_method_overload_for_types(gf, *args)
    if not f.ContainsGenericParameters:
        return gf
    return spec_generic_method(f, args, instance)


def call_generic_method(gf, *args):
    m = spec_generic_method_for(gf, *args)
    return m(args)


class GenericMethodProxy(object):
    def __init__(self, method):
        self.__method = method
        self.__cached = {}

    @property
    def __method__(self):
        return self.__method

    @property
    def cached(self):
        return self.__cached

    @staticmethod
    def __key(args):
        return tuple(type(x) for x in args)

    def __call__(self, *args):
        key = GenericMethodProxy.__key(args)
        method = self.__cached.get(key, None)
        if method is None:
            method = spec_generic_method_for_args(self.__method, *args)
            self.__cached[key] = method
        return method(*args)

    def __getitem(self, *args):
        method = self.__cached.get(args, None)
        if method is None:
            method = spec_generic_method_for_types(self.__method, *args)
            self.__cached[args] = method
        return method

    def __getitem__(self, arg):
        if isinstance(arg, int):
            return GenericMethodProxy(self.__method[arg])
        elif isinstance(arg, tuple):
            return self.__getitem(*arg)
        else:
            return self.__getitem(arg)

    def __getattr__(self, name):
        return getattr(self.__method, name)



def __wrap_generic_method(obj, name, get):
    value = get(obj, name)
    if isinstance(value, _dotnet.Interop.CallableBase):
        if contains_generic_overloads(value):
            return GenericMethodProxy(value)
    elif isinstance(value, _dotnet.Interop.Object):
        return value
    return value


def install():
    install_getattrhook(__wrap_generic_method)

