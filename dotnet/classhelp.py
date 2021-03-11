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

import pydoc
import dotnet.moduleloader

from dotnet import PyDotnet as _dotnet
from dotnet.asmresolve import assemblies
from dotnet.basics import clr

from System.Reflection import BindingFlags, MemberTypes
from System import Void

#
# Using python-with-types notation as described on: 
# https://www.python.org/dev/peps/pep-0484/
#


class _DocBuf:
    def __init__(self):
        self.__buffer = []

    def writeln(self, *args):
        s = map(str, args)
        self.__buffer.append(' '.join(s))

    def present(self):
        s = '\n'.join(self.__buffer)
        pydoc.pager(s)


def get_class_members(C):
    flags = BindingFlags.FlattenHierarchy 
    flags |= BindingFlags.Public
    flags |= BindingFlags.Instance 
    flags |= BindingFlags.Static
    members = C.__typeid__.GetMembers(flags)
    return members


def get_assembly_name(asmbly):
    fn = asmbly.FullName
    i = fn.find(',')
    return fn[:i] if i != -1 else fn


def get_generic_type_name(tn):
    i = tn.find('`')
    return tn[:i] if i != -1 else tn


def get_type_signature(ti, doc, refs):
    if ti.IsGenericType:
        tn = get_generic_type_name(ti.Name)
    else:
        tn = ti.Name
    refs.setdefault(ti.Assembly.FullName, set([])).add((ti, tn))
    if ti.IsGenericType:
        if ti.ContainsGenericParameters:
            ga = map(lambda x: x.Name, ti.GenericTypeParameters)
        else:
            ga = map(lambda sti: get_type_signature(sti, doc, refs), ti.GenericTypeArguments)
        return '%s[%s]' % (tn, ', '.join(ga))
    return '%s' % (tn,)
  

def get_parameter_signature(pi, doc, refs):
    return '%s: %s' % (pi.Name, get_type_signature(pi.ParameterType, doc, refs))


def get_parameterinstance_signature(piwt, doc, refs):
    return '%s: %s' % (piwt[0].Name, get_type_signature(piwt[1], doc, refs))


def get_property_signature(pi, doc, refs):
    pfx = ''
    if pi.CanRead and not pi.CanWrite:
        pfx = 'readonly '
    elif pi.CanWrite and not pi.CanRead:
        pfx = 'writeonly '
    return '%s%s: %s' % (pfx, pi.Name, get_type_signature(pi.PropertyType, doc, refs))


def get_field_signature(pi, doc, refs):
    pfx = ''
    if pi.IsStatic:
        pfx += 'static '
    if pi.IsInitOnly:
        pfx += ' readonly'
    return '%s%s: %s' % (pfx, pi.Name, get_type_signature(pi.FieldType, doc, refs))


def get_event_signature(ei, doc, refs):
    return 'event %s: %s' % (ei.Name, get_type_signature(ei.EventHandlerType, doc, refs))


def get_method_signature(mi, doc, refs, ptypes = None):
    rt = ''
    if mi.IsConstructor:
        mn = '__init__'
        rt = ' -> ' + get_type_signature(mi.ReflectedType, doc, refs)
    else:
        mn = mi.Name
        if not mi.ReturnType == Void.__typeid__:
            rt = ' -> ' + get_type_signature(mi.ReturnType, doc, refs)
        else:
            rt = ''
    
    if ptypes is None:
        ps = map(lambda pi: get_parameter_signature(pi, doc, refs), mi.GetParameters())
    else:
        ps = map(lambda piwt: get_parameterinstance_signature(piwt, doc, refs), zip(mi.GetParameters(), ptypes))

    if mi.IsStatic:
        return 'static %s(%s)%s' % (mn, ', '.join(ps), rt)
    else:
        return '%s(%s)%s' % (mn, ', '.join(ps), rt)


def get_class_help(C, doc, refs):
    ti = C.__typeid__
    all_members = get_class_members(C)

    def sorted_filtered(filter_expr, seq):
        return sorted(filter(filter_expr, seq), key=lambda x: x.Name)

    members = sorted_filtered(lambda x: not x.IsSpecialName, all_members)
    constructors = sorted_filtered(lambda x: x.MemberType == MemberTypes.Constructor, all_members)
    methods = sorted_filtered(lambda x: x.MemberType == MemberTypes.Method and not x.IsStatic, members)
    class_methods = sorted_filtered(lambda x: x.MemberType == MemberTypes.Method and x.IsStatic, members)
    properties = sorted_filtered(lambda x: x.MemberType == MemberTypes.Property, members)
    fields = sorted_filtered(lambda x: x.MemberType == MemberTypes.Field, members)
    events = sorted_filtered(lambda x: x.MemberType == MemberTypes.Event, members)
    nested_types = sorted_filtered(lambda x: x.MemberType == MemberTypes.NestedType, members)

    doc.writeln('Help on class %s in module %s:' % (get_type_signature(ti, doc, refs), get_assembly_name(ti.Assembly)))
    doc.writeln('')
    doc.writeln('class ' + get_type_signature(ti, doc, refs))
    if constructors or methods:
        doc.writeln(' |')
        doc.writeln(' |  Methods defined here:')
        doc.writeln(' |')
        for x in constructors:
            doc.writeln(' |  ' + get_method_signature(x, doc, refs))
            doc.writeln(' |')
        for x in methods:
            doc.writeln(' |  ' + get_method_signature(x, doc, refs))
            doc.writeln(' |')
    if class_methods:
        doc.writeln(' |  ----------------------------------------------------------------------')
        doc.writeln(' |  Class methods defined here:')
        doc.writeln(' |')
        for x in class_methods:
            doc.writeln(' |  ' + get_method_signature(x, doc, refs))
            doc.writeln(' |')
    if properties or fields or events:
        doc.writeln(' |  ----------------------------------------------------------------------')
        doc.writeln(' |  Data descriptors defined here:')
        doc.writeln(' |')
        for x in properties:
            doc.writeln(' |  ' + get_property_signature(x, doc, refs))
            doc.writeln(' |')
        for x in fields:
            doc.writeln(' |  ' + get_field_signature(x, doc, refs))
            doc.writeln(' |')
        for x in events:
            doc.writeln(' |  ' + get_event_signature(x, doc, refs))
            doc.writeln(' |')
    if nested_types:
        doc.writeln(' |  ----------------------------------------------------------------------')
        doc.writeln(' |  Data and other attributes defined here:')
        doc.writeln(' |')
        for x in nested_types:
            doc.writeln(' |  class ' + get_type_signature(x, doc, refs))
            doc.writeln(' |')
            doc.writeln(' |')


def get_method_help(M, doc, refs):
    mis = M.__func__
    if not isinstance(mis, list):
        mis = [mis]
    if hasattr(M, '__paramtypes__'):
        ptypes = M.__paramtypes__
    else:
        ptypes = None
    dt = mis[0].DeclaringType
    dts = get_type_signature(dt, doc, refs)
    asmn = get_assembly_name(dt.Assembly)
    doc.writeln('Help on method %s.%s in module %s:' % (dts, M.Name, asmn))
    doc.writeln('  ')
    for mi in mis:
        doc.writeln('  ' + get_method_signature(mi, doc, refs, ptypes))
        doc.writeln('  ')


def get_namespace_help(ns, doc, refs):
    doc.writeln('Help on namespace %s:' % (ns,))
    doc.writeln('')
    doc.writeln('namespace ' + str(ns))
    doc.writeln(' |')
    doc.writeln(' |  Data and other attributes defined here:')
    doc.writeln(' |')
    for x in sorted(dir(ns)):
        ti = ns[x].__typeid__
        if not ti.IsNestedPrivate:
            doc.writeln(' |  class ' + get_type_signature(ti, doc, refs))


def get_appdomain_help(clr, doc, refs):
    doc.writeln('Help on AppDomain')
    doc.writeln('')
    doc.writeln('AppDomain')
    doc.writeln(' |')
    doc.writeln(' |  Namespaces defined here:')
    doc.writeln(' |')
    hidden = ['boost', 'msclr', 'std', '<', '>']
    for ns in clr.Namespaces:
        if not any(x in ns for x in hidden):
            doc.writeln(' |  namespace ' + str(ns))
    doc.writeln('')
    doc.writeln('Available assemblies:')
    doc.writeln(' |')
    for asn in sorted(assemblies()):
        doc.writeln(' |  assembly ' + str(asn))


def get_module_help(mod, doc, refs):
    ns = clr[mod.__name__]
    return get_namespace_help(ns, doc, refs)


def get_references_help(doc, refs):
    if not refs:
        return
    doc.writeln('')
    doc.writeln('References:')
    doc.writeln(' |')
    for asmblyname, deftypes in sorted(refs.items(), key=lambda x: x[0]):
        doc.writeln(' |  ----------------------------------------------------------------------')
        doc.writeln(' |  Types defined in assembly %s:' % (asmblyname,))
        doc.writeln(' |')
        d = {}
        for ti, tn in sorted(deftypes, key=lambda x: x[1]):
            d.setdefault(ti.Namespace, set([])).add(tn)
        doc.writeln(' |')
        for ns in sorted(d):
            doc.writeln(' |    within namespace %s:' % (ns,))
            doc.writeln(' |')
            for tn in sorted(d[ns]):
                doc.writeln(' |  class %s' % (tn,))
            doc.writeln(' |')
        doc.writeln(' |')


def get_help(x, doc = None, refs = None):
    if doc is None:
        doc = _DocBuf()
    if refs is None:
        refs = {}
    if isinstance(x, _dotnet.Interop.Type):
        get_class_help(x, doc, refs)
        get_references_help(doc, refs)
    elif isinstance(x, _dotnet.Interop.CallableBase):
        get_method_help(x, doc, refs)
        get_references_help(doc, refs)
    elif isinstance(x, _dotnet.Interop.AppDomain):
        get_appdomain_help(x, doc, refs)
    elif isinstance(x, _dotnet.Interop.Namespace):
        get_namespace_help(x, doc, refs)
    elif isinstance(x, _dotnet.Interop.Object):
        get_class_help(x, doc, refs)
        get_references_help(doc, refs)
    elif isinstance(x, dotnet.moduleloader.PyDotnetModule):
        get_module_help(x, doc, refs)
    elif hasattr(x, '__typeid__'):
        get_class_help(x, doc, refs)
        get_references_help(doc, refs)
    elif hasattr(x, '__func__'):
        get_method_help(x, doc, refs)
        get_references_help(doc, refs)
    else:
        raise NotImplementedError
    doc.writeln('')
    doc.present()



