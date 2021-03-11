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

import sys
import imp
import types
from dotnet import PyDotnet as _dotnet


class PyDotnetModule(types.ModuleType):
    def __init__(self, name, doc=None, loader=None, namespace=None):
        super(PyDotnetModule, self).__init__(name, doc)
        self.__file__ = 'dotnet:%s' % name
        self.__path__ = []
        self.__loader__ = loader
        self.__namespace__ = namespace
        self.__all__ = dir(namespace)

    def __getattr__(self, key):
        return getattr(self.__namespace__, key)


__load_namespace_hook = None
    

def install_namespace_load_hook(hook):
    global __load_namespace_hook
    if __load_namespace_hook is None:
        __load_namespace_hook = hook
    else:
        prev_hook = __load_namespace_hook
        __load_namespace_hook = lambda ns: (prev_hook(ns), hook(ns))
    

def _namespace_loaded(ns):
    if __load_namespace_hook is not None:
        __load_namespace_hook(ns)


class PyDotnetLoader(object):
    def __init__(self, namespace):
        self.sys = sys
        self.imp = imp
        self.namespace = namespace


    def load_module(self, name):
        try:
            return self.sys.modules[name]
        except KeyError:
            pass
        try:
            target = self.namespace[name]
        except:
            raise ImportError('Cannot import %r from %r' % (name, self.namespace))
        if isinstance(target, _dotnet.Interop.Namespace):
            module = PyDotnetModule(name, loader=self, namespace=target)
            module = self.sys.modules.setdefault(name, module)
            _namespace_loaded(target)
            return module
        else:
            return target

    def find_module(self, name, path=None):
        try:
            member = self.namespace[name]
            return self
        except AttributeError:
            return None
        except:
            raise ImportError('Cannot import %r from %r' % (name, self.namespace))


__path__ = []
sys.meta_path.append(PyDotnetLoader(_dotnet.GlobalNamespace))


del sys
del imp
del types

