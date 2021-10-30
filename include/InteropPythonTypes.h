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

#ifndef INCLUDED_PYDOTNET_INTEROP_PYTHON_TYPES_H
#define INCLUDED_PYDOTNET_INTEROP_PYTHON_TYPES_H
#pragma managed(push)
#pragma unmanaged

#include <boost/function.hpp>
//#include <boost/algorithm/string_regex.hpp>
#include <boost/concept_check.hpp>
#include <boost/python.hpp>
#include <boost/python/slice.hpp>
#include <boost/python/str.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <set>


namespace InteropPython {

	extern bool g_DebugModuleInit;
	extern bool g_DebugDynamicInvokes;
	
	inline bool hasattr(boost::python::object obj, std::string const &attrName) 
	{
		return PyObject_HasAttrString(obj.ptr(), attrName.c_str());
	}

	inline std::string FriendlyTypename(const std::string& s)
	{
		return s;//boost::regex_replace(s, boost::regex("([,<])"), "\\1\n\t");
	}

	template<typename T>
	inline std::string FriendlyTypename()
	{
		return FriendlyTypename(typeid(T).name());
	}

	template<typename T>
	inline boost::python::object Repr(const T &x)
	{
		boost::python::object obj(x);
		PyObject *pobj = obj.ptr();
		PyObject *pres = PyObject_Repr(pobj);
		return boost::python::object(boost::python::detail::new_reference(pres));
	}

	template<typename T>
	inline std::string ReprString(const T &x)
	{
		boost::python::object r = Repr(x);
		return boost::python::extract<std::string>(x);
	}

	// It is VERY important to note that you MUST NOT touch any python code, or 
	// python data or call in to the interpreter while not holding the GIL. 
	// This will cause your interpreter to crash.
	// http://stackoverflow.com/questions/8009613/boost-python-not-supporting-parallelism
	struct AcquireGIL
	{
		AcquireGIL()
		{
			_state = PyGILState_Ensure();
		}

		~AcquireGIL()
		{
			PyGILState_Release(_state);
		}

	private:
		PyGILState_STATE _state;
	};

	// One of the advantages of Boost.Python is that you can release the GIL, do C++ stuff, 
	// and then take it back when you are done.
	struct ReleaseGIL
	{
		ReleaseGIL()
		{
			_state = PyEval_SaveThread();
		}

		~ReleaseGIL()
		{
			PyEval_RestoreThread(_state);
		}

	private:
		PyThreadState *_state;
	};

	typedef unsigned __int64 UInt64;
	typedef unsigned __int32 UInt32;
	typedef unsigned __int16 UInt16;
	typedef unsigned __int8 UInt8;
	typedef __int64 Int64;
	typedef __int32 Int32;
	typedef __int16 Int16;
	typedef __int8 Int8;
	typedef double Double;
	typedef float Single;
	typedef bool Boolean;

} // namespace InteropPython

#pragma managed(pop)
#endif // INCLUDED...
