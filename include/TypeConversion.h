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

#ifndef INCLUDED_PYDOTNET_TYPE_CONVERSION_H
#define INCLUDED_PYDOTNET_TYPE_CONVERSION_H

#include "InteropPythonTypes.h"
#include "InteropPythonExceptions.h"
#include "ManagedReferences.h"

#define PYDOTNET_PRINT_DEBUG(TEXT) PrintLine(std::string("DBG> ") + TEXT)

#define PYDOTNET_HANDLE_MANAGED_EXCEPTION(VARNAME) \
	catch (System::Exception ^VARNAME) { \
	std::string msg = ConvertToUnmanaged(VARNAME->ToString()); \
	throw_exception(msg); \
	throw std::runtime_error(msg); }


namespace InteropPython {

	// Convert managed type represented by System::Object ^ into unmanaged type
	template<typename FromType_, typename ToType_> struct FromManagedTypeConverter;

	// Convert unmanaged type into managed type represented by System::Object ^
	template<typename FromType_, typename ToType_> struct ToManagedTypeConverter;
	
	// Converter specialization(s)
	template<> struct FromManagedTypeConverter<System::String ^, std::string>;
	template<> struct FromManagedTypeConverter<System::Object ^, boost::python::object>;
	template<> struct ToManagedTypeConverter<std::string, System::String ^>;
	template<> struct ToManagedTypeConverter<boost::python::object, System::Object ^>;

	template<typename FromType_, typename ToType_>
	struct TypeConverterBase
	{
		typedef FromType_ FromType;
		typedef ToType_ ToType;
	};

	template<typename ManagedType_, typename UnamangedType_>
	struct ManagedTypeConversionInfo
	{
		typedef ManagedType_ ManagedType;
		typedef UnamangedType_ UnmanagedType;
	};

	// Generic type converter (from managed)
	// FromType --> ToType
	template<typename FromType_, typename ToType_>
	struct FromManagedTypeConverter : TypeConverterBase<FromType_, ToType_>
	{
		static ToType Convert(System::Object ^value)
		{
			if (value == nullptr)
				throw_null_reference(FriendlyTypename<FromManagedTypeConverter>() + "::Convert(null) not allowed!");
			return safe_cast<FromType>(value);
		}
	};

	// Generic type converter (to managed)
	// FromType --> ToType
	template<typename FromType_, typename ToType_>
	struct ToManagedTypeConverter : TypeConverterBase<FromType_, ToType_>
	{
		static ToType Convert(const FromType &fromValue, System::Type ^resultType)
		{
			return fromValue;
		}
	};

	// Generic type converter (not implemented)
	// Throws exception at runtime when used
	template<typename FromType_, typename ToType_>
	struct NotImplementedTypeConverter : TypeConverterBase<FromType_, ToType_>
	{
		static ToType Convert(...)
		{
			throw_invalid_cast(FriendlyTypename<NotImplementedTypeConverter>() + "::Convert() not implemented!");
			throw;
		}
	};

	// Generic type converter (no conversion)
	// FromType --> ToType
	template<typename FromType_, typename ToType_>
	struct PassThroughConverter : TypeConverterBase<FromType_, ToType_>
	{
		static ToType Convert(const FromType &fromValue)
		{
			return fromValue;
		}
	};

	////////////////////////////////////////////
	//
	// CONVERSION FUNCTIONS TO MANAGED TYPES
	//

	// C style zero terminated string --> managed string
	inline System::String ^ ConvertToManagedString(const char *cstr)
	{
		return gcnew System::String(cstr);
	}

	// C++ std::string --> managed string
	inline System::String ^ ConvertToManagedString(const std::string &str)
	{
		return gcnew System::String(str.c_str());
	}

	// Python list of str --> managed string[]
	inline array<System::String ^> ^ ConvertToManagedStringArray(const boost::python::list &lst)
	{
		using namespace boost::python;

		int n = len(lst);

		auto managedStrings = gcnew array<System::String ^>(n);

		for (int i = 0; i != managedStrings->Length; ++i)
		{
			std::string s = extract<std::string>(lst[i]);

			managedStrings[i] = ConvertToManagedString(s);
		}

		return managedStrings;
	}

	// Python list of float --> managed float[]
	inline array<System::Single> ^ ConvertToManagedSingleArray(const boost::python::list &lst)
	{
		using namespace boost::python;

		int n = len(lst);

		auto managedValues = gcnew array<System::Single>(n);

		for (int i = 0; i != managedValues->Length; ++i)
		{
			Single s = extract<Single>(lst[i]);

			managedValues[i] = s;
		}

		return managedValues;
	}

	// Python list of double --> managed double[]
	inline array<System::Double> ^ ConvertToManagedDoubleArray(const boost::python::list &lst)
	{
		using namespace boost::python;

		int n = len(lst);

		auto managedValues = gcnew array<System::Double>(n);

		for (int i = 0; i != managedValues->Length; ++i)
		{
			Double s = extract<Double>(lst[i]);

			managedValues[i] = s;
		}

		return managedValues;
	}

	// Python list of short --> managed short[]
	inline array<System::Int16> ^ ConvertToManagedInt16Array(const boost::python::list &lst)
	{
		using namespace boost::python;

		int n = len(lst);

		auto managedValues = gcnew array<System::Int16>(n);

		for (int i = 0; i != managedValues->Length; ++i)
		{
			Int16 s = extract<Int16>(lst[i]);

			managedValues[i] = s;
		}

		return managedValues;
	}

	// Python list of int --> managed int[]
	inline array<System::Int32> ^ ConvertToManagedInt32Array(const boost::python::list &lst)
	{
		using namespace boost::python;

		int n = len(lst);

		auto managedValues = gcnew array<System::Int32>(n);

		for (int i = 0; i != managedValues->Length; ++i)
		{
			Int32 s = extract<Int32>(lst[i]);

			managedValues[i] = s;
		}

		return managedValues;
	}

	// Python list of object --> managed object[]
	template<typename HandleType>
	inline array<typename HandleType::ObjectType ^> ^ ConvertToManagedObjectArray(const boost::python::list &objects) 
	{
		using namespace boost::python;

		int n = len(objects);
		auto managedObjects = gcnew array<typename HandleType::ObjectType ^>(n);

		for (int i = 0; i != managedObjects->Length; ++i)
		{
			HandleType handle = extract<HandleType>(objects[i]);

			managedObjects[i] = handle.GetObject();
		}

		return managedObjects;
	}

	// Python list of object --> managed List<object>
	template<typename HandleType>
	inline System::Collections::Generic::List<typename HandleType::ObjectType ^> ^
		ConvertToManagedObjectList(const boost::python::list &objects) 
	{
		using namespace boost::python;

		int n = len(objects);
		auto managedObjects = gcnew System::Collections::Generic::List<typename HandleType::ObjectType ^>(n);

		for (int i = 0; i != n; ++i)
		{
			HandleType handle = extract<HandleType>(objects[i]);

			managedObjects->Add(handle.GetObject());
		}

		return managedObjects;
	}

	// Python dict of object => object --> managed Dictionary<object, object>
	template<typename FromKeyType, typename FromValueType, typename ToKeyType, typename ToValueType>
	inline System::Collections::Generic::Dictionary<ToKeyType, ToValueType> ^ 
		ConvertToManagedDictionary(const boost::python::dict &dict_) 
	{
		using namespace boost::python;

		auto managedDict = gcnew System::Collections::Generic::Dictionary<ToKeyType, ToValueType>();

		list items = dict_.items();
		const int n = len(items);

		for (int i = 0; i != n; ++i)
		{
			tuple pair = extract<tuple>(items[i]);

			FromKeyType key = extract<FromKeyType>(pair[0]);
			FromValueType value = extract<FromValueType>(pair[1]);

			ToKeyType key2 = ToManagedTypeConverter<FromKeyType, ToKeyType>::Convert(key);
			ToValueType value2 = ToManagedTypeConverter<FromValueType, ToValueType>::Convert(value);

			managedDict->Add(key2, value2);
		}

		return managedDict;
	}

	// Python object --> managed System::Object	
	System::Object ^ ConvertToManagedObject(const boost::python::object &fromValue, System::Type ^resultType);

	////////////////////////////////////////////////////////
	//
	// CONVERSION FUNCTIONS TO UNMANAGED AND PYTHON TYPES
	//

	// Managed string --> C++ std::string
	inline std::string ConvertToUnmanaged(System::String ^str, bool allowEmptyString = false)
	{
		if (str == nullptr)
		{
			if (!allowEmptyString)
			{
				throw_null_reference("ConvertToUnmanaged(string: null) not allowed!");
			}
			return std::string();
		}

		msclr::interop::marshal_context context;
		return context.marshal_as<std::string>(str);
	}

	// Managed string[] --> python list
	inline boost::python::list ConvertToPython(array<System::String ^> ^managedStrings)
	{
		boost::python::list lst;

		if (managedStrings == nullptr)
			throw_null_reference("ConvertToPython(string[]: null) not allowed!");

		const int len = managedStrings->Length;

		for (int i = 0; i != len; ++i)
		{
			lst.append(ConvertToUnmanaged(managedStrings[i]));
		}

		return lst;
	}

	// Managed Single[] --> python list
	inline boost::python::list ConvertToPython(array<System::Single> ^managedValues)
	{
		boost::python::list lst;

		if (managedValues == nullptr)
			throw_null_reference("ConvertToPython(float[]: null) not allowed!");

		const int len = managedValues->Length;

		for (int i = 0; i != len; ++i)
		{
			Single value = managedValues[i];
			lst.append(value);
		}

		return lst;
	}

	// Managed Double[] --> python list
	inline boost::python::list ConvertToPython(array<System::Double> ^managedValues)
	{
		boost::python::list lst;

		if (managedValues == nullptr)
			throw_null_reference("ConvertToPython(double[]: null) not allowed!");

		const int len = managedValues->Length;

		for (int i = 0; i != len; ++i)
		{
			Double value = managedValues[i];
			lst.append(value);
		}

		return lst;
	}

	// Managed Int16[] --> python list
	inline boost::python::list ConvertToPython(array<System::Int16> ^managedValues)
	{
		boost::python::list lst;

		if (managedValues == nullptr)
			throw_null_reference("ConvertToPython(short[]: null) not allowed!");

		const int len = managedValues->Length;

		for (int i = 0; i != len; ++i)
		{
			Int16 value = managedValues[i];
			lst.append(value);
		}

		return lst;
	}

	// Managed Int32 --> python list
	inline boost::python::list ConvertToPython(array<System::Int32> ^managedValues)
	{
		boost::python::list lst;

		if (managedValues == nullptr)
			throw_null_reference("ConvertToPython(int[]: null) not allowed!");

		const int len = managedValues->Length;

		for (int i = 0; i != len; ++i)
		{
			Int32 value = managedValues[i];
			lst.append(value);
		}

		return lst;
	}

	// Managed object[] --> python list
	template<typename ObjectType>
	inline boost::python::list ConvertToPython(array<ObjectType> ^managedValues)
	{
		boost::python::list lst;

		if (managedValues == nullptr)
			throw_null_reference("ConvertToPython(object[]: null) not allowed!");

		const int len = managedValues->Length;

		for (int i = 0; i != len; ++i)
		{
			ObjectHandle value(managedValues[i]);
			lst.append(value);
		}

		return lst;
	}

	// Managed List<object> --> python list
	template<typename ObjectType>
	inline boost::python::list ConvertToPython(System::Collections::Generic::List<ObjectType> ^managedValues)
	{
		boost::python::list lst;

		if (managedValues == nullptr)
			throw_null_reference("ConvertToPython(list<object>: null) not allowed!");

		const int len = managedValues->Count;

		for (int i = 0; i != len; ++i)
		{
			ObjectHandle value(managedValues[i]);
			lst.append(value);
		}

		return lst;
	}

	// Managed Dictionary<object, object> --> python dict
	template<typename ToKeyType, typename ToValueType, typename FromKeyType, typename FromValueType>
	inline boost::python::dict ConvertToPython(System::Collections::Generic::Dictionary<FromKeyType, FromValueType> ^managedDict)
	{
		using namespace boost::python;

		dict dict_;

		if (managedDict == nullptr)
			throw_null_reference("ConvertToPython(dict<k,v>: null) not allowed!");

		typedef System::Collections::Generic::KeyValuePair<FromKeyType, FromValueType> EntryType;

		IEnumerator<EntryType> ^iter = managedDict->GetEnumerator();

		while (iter->MoveNext())
		{
			System::Object ^currentKey = iter->Current.Key;
			System::Object ^currentValue = iter->Current.Value;
			ToKeyType key = FromManagedTypeConverter<FromKeyType, ToKeyType>::Convert(currentKey);
			ToValueType value = FromManagedTypeConverter<FromValueType, ToValueType>::Convert(currentValue);

			dict_[key] = value;
		}

		return dict_;
	}


	/////////////////////////
	// DEBUG HELPERS

	inline void PrintLine(const std::string& text)
	{
		System::Console::Out->WriteLine(ConvertToManagedString(text));
	}

	inline void PrintError(const std::string& text)
	{
		System::Console::Error->WriteLine(ConvertToManagedString(text));
	}

} // namespace InteropPython

#endif // INCLUDED...
