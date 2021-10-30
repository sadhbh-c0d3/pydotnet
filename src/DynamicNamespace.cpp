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

	boost::python::object DynamicNamespace::GetMember(const std::string &name) const
	{
		std::string qname = GetQualifiedName(name);

		auto t = DynamicTypesCache::GetInstance().Types.find(qname);
		if (t != DynamicTypesCache::GetInstance().Types.cend())
		{
			return boost::python::object(t->second);
		}

		auto n = DynamicTypesCache::GetInstance().Namespaces.find(qname);
		if (n != DynamicTypesCache::GetInstance().Namespaces.end())
		{
			return boost::python::object(DynamicNamespace(qname));
		}

		throw_invalid_attribute("No such member: " + qname);
		throw std::runtime_error("No such member: " + qname);
	}

	boost::python::list DynamicNamespace::GetMembers() const
	{
		boost::python::list lst;

		for (auto t = DynamicTypesCache::GetInstance().Types.cbegin();
			t != DynamicTypesCache::GetInstance().Types.cend(); ++t)
		{
			System::Type ^typ = t->second.GetTypeObject();

			std::string ns = ConvertToUnmanaged(typ->Namespace, true);
			if (ns == _name)
			{
				std::string name = ConvertToUnmanaged(typ->Name);
				lst.append(name);
			}
		}

		return lst;
	}

	void DynamicNamespace::ImportTypesInto(boost::python::object &targetNamespace, const boost::python::object &names) const
	{
		//static const std::string patternString("[A-Za-z][A-Za-z0-9_]*(`[0-9]+)?");
		//static const boost::regex pattern(patternString);

		for (auto t = DynamicTypesCache::GetInstance().Types.cbegin(); t != DynamicTypesCache::GetInstance().Types.cend(); ++t)
		{
			System::Type ^typ = t->second.GetTypeObject();

			std::string ns = ConvertToUnmanaged(typ->Namespace, true);
			if (ns == _name)
			{
				std::string name = ConvertToUnmanaged(typ->Name);

				// if (!boost::regex_match(name, pattern))
				// {
				// 	// Filter out names that are not user defined symbols, e.g. '<>c__DisplayClass1already'
				// 	PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG(name + std::string(" does not match ") + patternString);
				// 	continue;
				// }
				//
				if (name.find("<") == 0){ 
					continue;
				}


				if (t->first.find("`") != std::string::npos)
				{
					// Filter out full generic names
					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG(name + std::string(" is full generic name"));
					continue;
				}

				int pos = name.find("`");
				if (pos != std::string::npos)
				{
					name = name.substr(0, pos);
				}

				if (!names.is_none() && !names.contains(name))
				{
					continue;
				}

				if (targetNamespace.contains(name.c_str()))
				{
					throw_exception(name + std::string(" already defined"));
					throw std::runtime_error(name + std::string(" already defined"));
				}
				targetNamespace[name.c_str()] = DynamicTypeHandle(typ);
			}
		}
	}

} // namespace InteropPython
