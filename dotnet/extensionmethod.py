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
from dotnet.basics import install_getattrhook
from dotnet.moduleloader import install_namespace_load_hook
from dotnet.genericmethod import GenericMethodProxy, contains_generic_overloads

import System


def find_extension_methods_from_type(T):
    for mi in T.__typeid__.GetMethods():
        if mi.IsDefined(System.Runtime.CompilerServices.ExtensionAttribute, False):
           extended_T = mi.GetParameters()[0].ParameterType
           if extended_T.IsGenericParameter:
               extended_T = System.Object.__typeid__
           elif extended_T.IsGenericType:
               extended_T = extended_T.GetGenericTypeDefinition()
           yield extended_T, mi


def find_extension_methods_from_namespace(ns):
    for tname in dir(ns):
        T = ns[tname]
        for x in find_extension_methods_from_type(T):
            yield x


def build_extended_T_key(extended_T):
   return (extended_T.Namespace, extended_T.Name)


def build_extension_methods_lookup(seq):
    extmap = {}
    for extended_T, mi in seq:
        key = build_extended_T_key(extended_T)
        pmap = extmap.setdefault(key, {})
        pmap.setdefault(mi.Name, []).append(mi)
    return extmap


class ExtensionMethodProxy(object):
    def __init__(self, extension, obj):
        if isinstance(extension, _dotnet.Interop.CallableBase):
            self.__method = extension
        else:
            overloads = list(map(_dotnet.Interop.Method, extension))
            if len(overloads) is 1:
                self.__method = overloads[0]
            else:
                self.__method = _dotnet.Interop.MethodOverloads()
                for method in overloads:
                    self.__method.AddOverload(method)
        if contains_generic_overloads(self.__method):
            self.__method = GenericMethodProxy(self.__method)
        self.__obj = obj

    @property
    def __method__(self):
        return self.__method

    @property
    def __target__(self):
        return self.__obj

    def __call__(self, *args):
        args = (self.__obj,) + args
        return self.__method(*args)

    def __getitem__(self, arg):
        method = self.__method[arg]
        return ExtensionMethodProxy(method, self.__obj)

    def __getattr__(self, name):
        return getattr(self.__method, name)


class ExtensionMethods(object):
    def __init__(self):
        self.__registry = {}
        self.__cached = {}

    def add(self, extmap):
        self.__registry.update(extmap)

    def __tryget(self, extended_T, name):
        # Try type it-self
        key = build_extended_T_key(extended_T)
        if key in self.__registry:
            pmap = self.__registry[key]
            if name in pmap:
                return pmap[name]
        # Try interfaces
        ifaces = extended_T.GetInterfaces()
        if ifaces is not None:
            for iface in ifaces:
                res = self.__tryget(iface, name)
                if res is not None:
                    return res
        # Try base
        base = extended_T.BaseType
        if base is not None:
            return self.__tryget(base, name)
        return None

    def get(self, obj, name):
        extended_T = obj.__typeid__
        if extended_T.IsGenericType:
            extended_T = extended_T.GetGenericTypeDefinition()
        key = (extended_T, name)
        res = self.__cached.get(key, None)
        if res is None:
            res = self.__tryget(extended_T, name)
        if res is None:
            raise AttributeError
        self.__cached[key] = res
        return ExtensionMethodProxy(res, obj)

    def __iter__(self):
        return iter(self.__registry.items())


__extensions = None


def extensions():
    global __extensions
    if __extensions is None:
        return iter(())
    return iter(__extensions)


def __register_extension_methods(ns):
    global __extensions
    if __extensions is None:
        __extensions = ExtensionMethods() 
    seq = find_extension_methods_from_namespace(ns)
    extmap = build_extension_methods_lookup(seq)
    __extensions.add(extmap)

    
def __wrap_extension_method(obj, name, get):
    global __extensions
    try:
        return get(obj, name)
    except AttributeError:
        if __extensions is None:
            raise AttributeError
        return __extensions.get(obj, name)


def install():
    install_getattrhook(__wrap_extension_method)
    install_namespace_load_hook(__register_extension_methods)
