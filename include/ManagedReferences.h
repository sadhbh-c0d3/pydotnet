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

#ifndef INCLUDED_PYDOTNET_MANAGED_REFERENCES_H
#define INCLUDED_PYDOTNET_MANAGED_REFERENCES_H
#pragma managed(push)
#pragma managed

#using <System.Core.dll>
#using <System.Data.dll>
#using <Microsoft.CSharp.dll>

#include <vcclr.h>
#include <msclr\marshal_cppstd.h>

namespace InteropPython 
{
	using System::Enum;
	using System::Convert;
	using System::Collections::Generic::IEnumerator;
	using System::Collections::Generic::IEnumerable;
	using System::Collections::Generic::IList;
	using System::Collections::Generic::List;
	using System::Collections::Generic::IDictionary;
	using System::Collections::Generic::Dictionary;

	generic<class T> public ref class ImplicitConversion
	{
	public:
		static T Convert(T item)
		{
			// Implicit conversion will take Python object and turn it into .NET object.
			// For implicit convertion to work we need static function Convert(), the 
			// invocation of which will trigger type conversion.
			return item;
		}
	};

	generic<class T> public ref class ArrayHelper
	{
	public:
		static array<T> ^ CreateArray(int size)
		{
			return gcnew array<T>(size);
		}

		static array<T> ^ GetArray(array<T> ^elements)
		{
			// Implicit conversion will take Python list and turn it into array<T^>.
			// For implicit convertion to work we need static function GetArray(), the 
			// invocation of which will trigger type conversion.
			return elements;
		}

		static System::Type ^ GetArrayType()
		{
			return array<T>::typeid;
		}
	};
}

#pragma managed(pop)
#endif // INCLUDED...
