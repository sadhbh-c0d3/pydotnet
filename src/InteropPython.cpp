// The MIT License (MIT) Copyright (c) 2016, Susquehanna International Group
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "InteropPython.h"

namespace InteropPython {

	bool g_DebugModuleInit = false;
	bool g_DebugDynamicInvokes = false;

	void InitDebugOptions()
	{
		boost::python::object main = boost::python::import("__main__");
		boost::python::object main_namespace = main.attr("__dict__");
		
		if (main_namespace.contains("debug_ModuleInit"))
		{
			boost::python::object val = main.attr("debug_ModuleInit");
			g_DebugModuleInit = boost::python::extract<bool>(val);
		}

		if (main_namespace.contains("debug_DynamicInvokes"))
		{
			boost::python::object val = main.attr("debug_DynamicInvokes");
			g_DebugDynamicInvokes = boost::python::extract<bool>(val);
		}
	}

	void NamespaceScope::Register(const std::string &name)
	{
		InitDebugOptions();

		using namespace boost::python;
	
		scope scope_ = class_<NamespaceScope>(name.c_str());

		DynamicTypesCache::Register("TypesCache");
		ObjectHandle::Register("ObjectBase");
		DynamicObjectHandle::Register("Object");
		DynamicIterator::Register("Iterator");
		InvocationForwarding::Register("CallableBase");
		DynamicCallable::Register("Callable");
		DynamicCallableInstance::Register("CallableInstance");
		DynamicMethodInvoker::Register("Method", "MethodOverloads");
		DynamicConstructorInvoker::Register("Constructor", "ConstructorOverloads");
		DynamicTypeHandle::Register("Type");
		DynamicNamespace::Register("Namespace");
		DynamicAppDomain::Register("AppDomain");
	}

} // namespace InteropPython
