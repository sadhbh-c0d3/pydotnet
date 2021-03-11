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
#include "PyDotnet.h"

BOOST_PYTHON_MODULE(PyDotnet)
{
	using namespace boost::python;

	// If you are going to be messing with the GIL in c++ your module definition needs to start with this code
	// http://stackoverflow.com/questions/8009613/boost-python-not-supporting-parallelism
	PyEval_InitThreads();

	// This will enable user-defined docstrings and python signatures,
	// while disabling the C++ signatures
	// http://stackoverflow.com/questions/6114462/how-to-override-the-automatically-created-docstring-data-for-boostpython
	docstring_options local_docstring_options(true, true, false);

	InteropPython::NamespaceScope::Register("Interop");

	boost::python::scope s;
	InteropPython::DynamicAppDomain app;
	boost::python::object appDomain(app);

	s.attr("TypesCache") = InteropPython::DynamicTypesCache::GetInstanceWrapper();
	s.attr("GlobalNamespace") = appDomain;
	s.attr("Import") = appDomain.attr("LoadAssembly");
	s.attr("Using") = appDomain.attr("ResolveUsing");
	
	def("IsInstance", &InteropPython::IsInstance);
	def("GetType", &InteropPython::GetType);
	def("GetClass", &InteropPython::GetClass);
	def("GetConstructor", &InteropPython::GetConstructor);
}
