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

	boost::python::object DynamicObjectDetail::DoGetProperty(System::Object ^obj, PropertyInfo ^pi)
	{
		System::Object ^value;
		{
			ReleaseGIL lk;
			value = pi->GetValue(obj, nullptr);
		}

		if (value == nullptr)
		{
			return boost::python::object();
		}

		return ConvertToPython(value, pi->PropertyType);
	}

	boost::python::object DynamicObjectDetail::DoGetProperty(System::Object ^obj, FieldInfo ^pi)
	{
		System::Object ^value = value = pi->GetValue(obj);

		if (value == nullptr)
		{
			return boost::python::object();
		}

		return ConvertToPython(value, pi->FieldType);
	}

	boost::python::object DynamicObjectDetail::DoGetProperty(System::Object ^obj, MethodInfo ^pi)
	{
		return boost::python::object(DynamicMethodInvoker(pi, obj));
	}

	boost::python::object DynamicObjectDetail::DoGetProperty(System::Object ^obj, System::Type ^pi)
	{
		return boost::python::object(DynamicTypeHandle(pi));
	}

	boost::python::object DynamicObjectDetail::DoGetProperty(System::Object ^obj, array<MemberInfo ^> ^mi)
	{
		if (mi->Length == 0)
		{
			return boost::python::object();
		}
		else if (mi->Length == 1)
		{
			return DoGetProperty(obj, mi[0]);
		}
		else if (mi[0]->MemberType == System::Reflection::MemberTypes::Method)
		{
			DynamicOverloadResolver<DynamicMethodInvoker> overloads;
			const int n = mi->Length;
			for (int i = 0; i != n; ++i)
			{
				MethodInfo ^mmi = safe_cast<MethodInfo^>(mi[i]);
				overloads.Add(DynamicMethodInvoker(mmi, obj));
			}
			return boost::python::object(overloads);
		}
		else
		{
			boost::python::list lst;
			const int n = mi->Length;
			for (int i = 0; i != n; ++i)
			{
				auto x = DoGetProperty(obj, mi[i]);
				lst.append(x);
			}
			return lst;
		}
	}

}//namespace InteropPython
