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

#ifndef INCLUDED_PYDOTNET_DYNAMIC_TYPE_CONVERTER_CHOICE_H
#define INCLUDED_PYDOTNET_DYNAMIC_TYPE_CONVERTER_CHOICE_H

#include "TypeConversion.h"

namespace InteropPython {

	struct DynamicObjectHandle;

	// Dynamically chooses specialized type converter 
	// Choice is based upon managed type user provides, and
	// user action is parametrized with specialized converter type, and
	// it is applied onto user parameters.
	struct DynamicTypeConverterChoice
	{
		template<template <typename> class UserAction, typename ActionArgs>
		static void Apply(System::Type ^typ, ActionArgs &&args)
		{
			if (typ->Equals(System::Byte::typeid))
			{
				UserAction<ManagedTypeConversionInfo<System::Byte, UInt8> >::Apply(std::forward<ActionArgs>(args));
			}
			else if(typ->Equals(System::UInt16::typeid))
			{
				UserAction<ManagedTypeConversionInfo<System::UInt16, UInt16> >::Apply(std::forward<ActionArgs>(args));
			}
			else if(typ->Equals(System::UInt32::typeid))
			{
				UserAction<ManagedTypeConversionInfo<System::UInt32, UInt32> >::Apply(std::forward<ActionArgs>(args));
			}
			else if(typ->Equals(System::UInt64::typeid))
			{
				UserAction<ManagedTypeConversionInfo<System::UInt64, UInt64> >::Apply(std::forward<ActionArgs>(args));
			}
			else if(typ->Equals(System::SByte::typeid))
			{
				UserAction<ManagedTypeConversionInfo<System::SByte, Int8> >::Apply(std::forward<ActionArgs>(args));
			}
			else if(typ->Equals(System::Int16::typeid))
			{
				UserAction<ManagedTypeConversionInfo<System::Int16, Int16> >::Apply(std::forward<ActionArgs>(args));
			}
			else if(typ->Equals(System::Int32::typeid))
			{
				UserAction<ManagedTypeConversionInfo<System::Int32, Int32> >::Apply(std::forward<ActionArgs>(args));
			}
			else if(typ->Equals(System::Int64::typeid))
			{
				UserAction<ManagedTypeConversionInfo<System::Int64, Int64> >::Apply(std::forward<ActionArgs>(args));
			}
			else if(typ->Equals(System::Boolean::typeid))
			{
				UserAction<ManagedTypeConversionInfo<System::Boolean, Boolean> >::Apply(std::forward<ActionArgs>(args));
			}
			else if(typ->Equals(System::Double::typeid))
			{
				UserAction<ManagedTypeConversionInfo<System::Double, Double> >::Apply(std::forward<ActionArgs>(args));
			}
			else if(typ->Equals(System::Single::typeid))
			{
				UserAction<ManagedTypeConversionInfo<System::Single, Single> >::Apply(std::forward<ActionArgs>(args));
			}
			else if(typ->Equals(System::String::typeid))
			{
				UserAction<ManagedTypeConversionInfo<System::String ^, std::string> >::Apply(std::forward<ActionArgs>(args));
			}
			else 
			{
				UserAction<ManagedTypeConversionInfo<System::Object ^, boost::python::object> >::Apply(std::forward<ActionArgs>(args));
			}
		}
	};


} // namespace InteropPython

#endif // INCLUDED...
