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

print()
print(":: PyDotnet :: Seamless ::")
print("==========================")
print("The module for integration of PyDotnet with Interactive Python")
print()
print("::: FEATURES :::")
print("================")
print("- Use help() on any .NET namespace, class, object, method at will")
print("- Use help(dotnet.clr) to obtain list of namespaces an assemblies")
print("- Use help() on imported namespaces, e.g.")
print("     import System.Collections")
print("     help(System.Collections)")
print()
print("- Use help() on types, e.g. `help(List)`")
print("- Use help() on constructors, e.g. `help(List.__createinstance__)`")
print("- Use help() on unbound methods, e.g. `help(List[Int32].FindIndex)`")
print("- Use help() on bound methods, e.g. `help(myList.FindIndex)`")
print()
print("- Use type(), isinstance(), issubclass() on .NET objects and types")
print()
print("- Generic method support is ON")
print("- Extension method support is ON")
print("- Assembly search parth use DEFAULT")
print()
print("!!! IMPORTANT !!!")
print("=================")
print("Please, remember to NOT USE 'import dotnet.seamless' IN PRODUCTION CODE. This")
print("module is provided for you, so you can use it when you browse around .NET world")
print("while in Python interactive mode.")
print()
print("!!! WARNING !!!")
print("===============")
print("This module overrides help(), type(), isinstance(), issubclass() methods. If")
print("you use this module in production code you may get unexpected behavior.")
print()
print("::: PRODUCTION CODE :::")
print("=======================")
print("Please, for production code use 'import dotnet' and:")
print(" - if you need to enable support for generic methods use:")
print("     dotnet.genericmethod.install()")
print("   (NOTE: make sure you call that ONLY ONCE)")
print()
print(" - if you need to enable support for extension methods use:")
print("     dotnet.extensionmethod.install()")
print("   (NOTE: make sure you call that ONLY ONCE)")
print()
print(" - if you want type(), isinstance(), and issubclass() then use:")
print("     from dotnet.overrides import type, isinstance, issubclass")
print("   (NOTE: you need to do that only in module, which uses these)")
print()
print(" - if you want to import common .NET types use:")
print("     from dotnet.commontypes import *")
print("   (NOTE: you need to do that in your module, which uses any of:")
print("    Action*, Func*, Array, Tuple*, Dictionary)")
print()
print(" - if you want to use default assembly search path then use:")
print("     dotnet.add_framework_assemblies()")
print()
print("::: QUICK START :::")
print("=======================")
print()
print(" - if you want to load assemblies matching name pattern then use for example:")
print("     dotnet.load_assemblies(\"Linq\")")
print()
print(" - if you want to load specific assembly then use for example:")
print("     dotnet.load_assembly(\"System.Linq\")")
print()
print(" - if you want to load assembly from file then use for example:")
print("     dotnet.load_assembly(\".\\MyAssembly.dll\")")
print()
print(" - if you want to import .NET namespace then use for example:")
print("     import System.Linq")
print()
print()



from dotnet import *

dotnet.genericmethod.install()
dotnet.extensionmethod.install()
dotnet.overrides.install()
dotnet.add_framework_assemblies()
