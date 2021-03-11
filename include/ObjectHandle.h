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

#ifndef INCLUDED_PYDOTNET_OBJECTHANDLE_H
#define INCLUDED_PYDOTNET_OBJECTHANDLE_H

#include "InteropPythonTypes.h"
#include "TypeConversion.h"
#include "ManagedReferences.h"

namespace InteropPython {

	struct DynamicObjectHandle;

	struct ObjectHandle
	{
		ObjectHandle() 
		{
		}

		ObjectHandle(System::Object ^obj) : _obj(obj) 
		{
		}

		ObjectHandle(const ObjectHandle &other) : _obj(other._obj) 
		{
		}

		void DeleteObject()
		{
			// According to http://stackoverflow.com/questions/350052/how-do-you-dispose-of-an-idisposable-in-managed-c
			// The 'delete' does dispose an object if it was defined as 'IDisposable'.
			delete _obj;
		}

		ObjectHandle& EnterObject()
		{
			return *this;
		}

		void ExitObject(boost::python::object type, boost::python::object value, boost::python::object traceback)
		{
			DeleteObject();
		}

		System::Object ^GetObject() const { return _obj; }

		virtual std::string GetTypeName() const
		{
			if (_obj.operator System::Object ^() == nullptr)
			{
				return std::string();
			}
			return ConvertToUnmanaged(_obj->GetType()->Name, true);
		}

		DynamicObjectHandle ToDynamicHandle() const;
	
		boost::python::object GetDynamicProperty(const char *name) const;

		boost::python::str ToString() const;
		
		boost::python::str ToReprString() const;
		
		boost::python::str ToPrettyString() const;

		virtual int GetHashCode() const
		{
			return GetObject()->GetHashCode();
		}

		Int32 ToInt32() const
		{
			return Convert::ToInt32(GetObject());
		}

		ObjectHandle& FromInt32(System::Int32 x)
		{
			System::Type ^typ = GetObject()->GetType();
			if (typ->IsEnum)
			{
				try 
				{
					_obj = Enum::ToObject(typ, x);
					return *this;
				}
				PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
			}
			throw_invalid_cast("Enum required");
			throw std::runtime_error("Enum required");
		}

		bool operator == (const ObjectHandle &b) const
		{
			return GetObject()->Equals(b.GetObject());
		}

		bool operator != (const ObjectHandle &b) const
		{
			return !((*this) == b);
		}

		ObjectHandle& operator |= (const ObjectHandle &b)
		{
			return FromInt32(ToInt32() | b.ToInt32());
		}

		ObjectHandle& operator &= (const ObjectHandle &b)
		{
			return FromInt32(ToInt32() & b.ToInt32());
		}

		ObjectHandle& operator ^= (const ObjectHandle &b)
		{
			return FromInt32(ToInt32() ^ b.ToInt32());
		}

		ObjectHandle& operator ~ ()
		{
			return FromInt32(!ToInt32());
		}

		static void Register(const std::string &name)
		{
			using namespace boost::python;

			class_<ObjectHandle>(name.c_str(), no_init)
				.add_property("__typename__", &GetTypeName, "Gets name of managed type")
                
                // ObjectHandle is a base class for all references to .NET objects held within Python
                // Not all of them are held by DynamicObjectHandle, however one may convert any ObjectHandle into
                // DynamicObjectHandle and access attributes (and methods) via C# Reflection mechanism.
				.add_property("__dynamic__", &ToDynamicHandle, "Gets object whose properties are resolved at run-time")

                // Getting attributes of the objects is done dynamically via C# Reflection
				.def("__getattr__", &GetDynamicProperty, "Gets field, property, method, method overloads or nested type from managed type")
				
                // Python standard to string conversions plus one extra that gives some prettifies information
                .def("__str__", &ToString, "Formats string")
				.def("__repr__", &ToReprString, "Formats representation string")
				.def("__pretty__", &ToPrettyString, "Formats pretty representation string")

                // NOTE: deliberately not __del__ as that would delete .NET object when least expected
                // User needs to call __delete__() manually in order to dispose of the resources.
                // NOTE: Use python 'with' statement instead, as its behavior is more predictable, and thus
                // we can automatically dispose on __exit__ call.
				.def("__delete__", &DeleteObject, "Destroy allocated resources") 
				.def("__enter__", &EnterObject, "Enter with", return_internal_reference<>())
				.def("__exit__", &ExitObject, "Exit with")

                // Conversion to int and hash code
                // NOTE: Python defines these two quite separately
				.def("__int__", &ToInt32, "Gets integer value")
				.def("__hash__", &GetHashCode, "Get hash")

                // Some traditional operator overloads
                // NOTE: Result operation is ObjectHandle and may not match target type.
                // Use __dynamic__ to obtain DynamicObjectHandle, which will most likely fit target type.
				.def(self == self)
				.def(self != self)
				.def(self |= self)
				.def(self | self)
				.def(self &= self)
				.def(self & self)
				.def(self ^= self)
				.def(self ^ self)
				.def(~self)
				;
		}

	private:
		gcroot<System::Object ^> _obj;

		ObjectHandle(ObjectHandle &&other);
	};

	inline ObjectHandle operator | (const ObjectHandle &a, const ObjectHandle &b)
	{
		ObjectHandle tmp(a); return tmp |= b;
	}

	inline ObjectHandle operator & (const ObjectHandle &a, const ObjectHandle &b)
	{
		ObjectHandle tmp(a); return tmp &= b;
	}

	inline ObjectHandle operator ^ (const ObjectHandle &a, const ObjectHandle &b)
	{
		ObjectHandle tmp(a); return tmp ^= b;
	}

} // InteropPython

#endif // INCLUDED...
