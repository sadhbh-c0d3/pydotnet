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

#ifndef INCLUDED_PYDOTNET_TYPE_CONVERTER_SPECIALIZATIONS_H
#define INCLUDED_PYDOTNET_TYPE_CONVERTER_SPECIALIZATIONS_H

#include "DynamicObjectHandle.h"

namespace InteropPython {

	////////////////////////////////////////////////////////
	//
	// SPECIALIZED FROM MANAGED TO UNMANAGED TYPE CONVERTERS
	//

	// Specialized type converter
	// Managed string --> C++ std::string
	template<> struct FromManagedTypeConverter<System::String ^, std::string> : TypeConverterBase<System::String ^, std::string>
	{
		static ToType Convert(System::Object ^value)
		{
			if (value == nullptr)
				throw_null_reference(FriendlyTypename<FromManagedTypeConverter>() + "::Convert(null) not allowed!");
			try
			{
				FromType fromValue = safe_cast<FromType>(value);
				ToType toValue = ConvertToUnmanaged(fromValue);
				return toValue;
			}
			catch(System::Exception ^err)
			{
				throw_invalid_cast(FriendlyTypename<FromManagedTypeConverter>() + "::Convert(null) failed!\n" + 
					ConvertToUnmanaged(err->ToString()));
				throw;
			}
		}
	};

	// Specialized type converter
	// Managed object --> python object
	template<> struct FromManagedTypeConverter<System::Object ^, boost::python::object> : TypeConverterBase<System::Object ^, boost::python::object>
	{
		static ToType Convert(System::Object ^value)
		{
			if (value == nullptr)
				throw_null_reference(FriendlyTypename<FromManagedTypeConverter>() + "::Convert(null) not allowed!");
			FromType fromValue = value;
			DynamicObjectHandle handle(fromValue);
			ToType toValue(handle);
			return toValue;
		}
	};

	////////////////////////////////////////////////////////
	//
	// SPECIALIZED FROM UNMANAGED TO MANAGED TYPE CONVERTERS
	//

	// Specialized type converter
	// C++ string --> managed string
	template<> struct ToManagedTypeConverter<std::string, System::String ^> : TypeConverterBase<std::string, System::String ^>
	{
		static ToType Convert(const FromType &fromValue, System::Type ^resultType)
		{
			ToType toValue = ConvertToManagedString(fromValue);
			return toValue;
		}
	};

	// Specialized type converter
	// python object -> managed object
	template<> struct ToManagedTypeConverter<boost::python::object, System::Object ^> : TypeConverterBase<boost::python::object, System::Object ^>
	{
		static ToType Convert(const FromType &fromValue, System::Type ^resultType)
		{
			return ConvertToManagedObject(fromValue, resultType);
		}
	};

	template<> struct DynamicObjectDetail::ConversionToManaged::Action<ManagedTypeConversionInfo<System::Object ^, boost::python::object> >
	{
		static void Apply(ConversionToManaged &args)
		{
			args.result = ToManagedTypeConverter<boost::python::object, System::Object ^>::Convert(args.value, args.resultType);
		}
	};

} // namespace InteropPython

#endif//INCLUDED...
