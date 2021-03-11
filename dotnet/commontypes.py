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

import dotnet.moduleloader

#
# This module imports most common types from .NET
#
# NOTE: Some of the common types like Action or Dictionary are provided as wrapped types by proxytypes module instead.
#


# Standard types
from System import Void, String, Object, Type

# Integer types
from System import Int16, Int32, Int64, UInt16, UInt32, UInt64, Byte, SByte

# Floating point types
from System import Single, Double, Decimal

# List
from System.Collections.Generic import List

# Tuple1..Tuple8, Action1..Action8, Func1..Func8, Array, Dictionary
from dotnet.proxytypes import *

def framework_path():
   return (String.__typeid__.Assembly.CodeBase)[len('file:///'):-len('/mscorlib.dll')]

