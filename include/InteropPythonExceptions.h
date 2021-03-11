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

#ifndef INCLUDED_PYDOTNET_INTEROP_PYTHON_EXCEPTIONS_H
#define INCLUDED_PYDOTNET_INTEROP_PYTHON_EXCEPTIONS_H
#pragma managed(push)
#pragma unmanaged

#include "InteropPythonTypes.h"

namespace InteropPython {

	inline void throw_stop_iteration(const std::string &msg = "No more data")
	{
		PyErr_SetString(PyExc_StopIteration, msg.c_str());
		boost::python::throw_error_already_set();
	}

	inline void throw_index_error(const std::string &msg = "Index out of range")
	{
		PyErr_SetString(PyExc_IndexError, msg.c_str());
		boost::python::throw_error_already_set();
	}

	inline void throw_invalid_cast(const std::string &msg = "Invalid cast")
	{
		PyErr_SetString(PyExc_TypeError, msg.c_str());
		boost::python::throw_error_already_set();
	}

	inline void throw_null_reference(const std::string &msg = "Null reference")
	{
		PyErr_SetString(PyExc_ReferenceError, msg.c_str());
		boost::python::throw_error_already_set();
	}

	inline void throw_invalid_attribute(const std::string &msg = "Invalid attribute")
	{
		PyErr_SetString(PyExc_AttributeError, msg.c_str());
		boost::python::throw_error_already_set();
	}

	inline void throw_exception(const std::string &msg = "Invalid attribute")
	{
		PyErr_SetString(PyExc_Exception, msg.c_str());
		boost::python::throw_error_already_set();
	}

} // namespace InteropPython

#pragma managed(pop)
#endif // INCLUDED...
