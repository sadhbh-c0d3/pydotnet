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

import dotnet
from dotnet.basics import install_getattrhook

from System.Reflection import MemberInfo, MemberTypes, BindingFlags


flags = BindingFlags.Instance
flags |= BindingFlags.NonPublic
flags |= BindingFlags.FlattenHierarchy


def bind_method(obj, method):
    return dotnet.PyDotnet.Interop.Method(method, obj)

def bind_field_get(obj, field):
    return lambda: field.GetValue(obj)

def bind_field_set(obj, field):
    return lambda val: field.SetValue(obj, val)

def bind_property_get(obj, prop):
    return bind_method(obj, prop.GetGetMethod())

def bind_property_private_get(obj, prop):
    return bind_method(obj, prop.GetGetMethod(True))

def bind_property_set(obj, prop):
    return bind_method(obj, prop.GetSetMethod())

def get_private_member(obj, name):
    return obj.__typeid__.GetMember(name, flags)

def get_private_members(obj):
    return obj.__typeid__.GetMembers(flags)

def wrap_member(obj, member):
    attr = {'name': member.Name}
    if member.MemberType == MemberTypes.Field:
        attr['get'] = bind_field_get(obj, member)
        attr['set'] = bind_field_set(obj, member)
    elif member.MemberType == MemberTypes.Property:
        try:
            attr['get'] = bind_property_get(obj, member)
        except:
            try:
                attr['get'] = bind_property_private_get(obj, member)
            except:
                pass
        try:
            attr['set'] = bind_property_set(obj, member)
        except:
            pass
    elif member.MemberType == MemberTypes.Method:
        attr['call'] = bind_method(obj, member)
    return attr


class PrivateMembers(object):
    def __init__(self, attrs):
        if isinstance(attrs, dotnet.PyDotnet.Interop.Object):
            attrs = [wrap_member(attrs, member) for member in get_private_members(attrs)]
        self.__attrs = dict((attr['name'], attr) for attr in attrs)

    def __getattr__(self, name):
        if name.startswith('_PrivateMembers__') or name.startswith('__'):
            return object.__getattr__(self, name)
        attr = self.__attrs[name]
        if 'get' in attr:
            return attr['get']()
        return attr['call']

    def __setattr__(self, name, value):
        if name.startswith('_PrivateMembers__') or name.startswith('__'):
            return object.__setattr__(self, name, value)
        attr = self.__attrs[name]
        return attr['set'](value)

    
def __wrap_private_members(obj, name, get):
    try:
        return get(obj, name)
    except AttributeError:
        if name == '__private__':
            return PrivateMembers(obj)
        raise AttributeError

def install():
    install_getattrhook(__wrap_private_members)


