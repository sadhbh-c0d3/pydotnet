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

	DynamicTypesCache *DynamicTypesCache::sInstance = nullptr;

	void DynamicTypesCache::Refresh1(System::Reflection::Assembly ^assembly)
	{
		std::string asmName = ConvertToUnmanaged(assembly->FullName);

		if (!asmName.empty())
		{
			if (!Assemblies.insert(asmName).second)
			{
				return;
			}
		}

		auto types = assembly->GetTypes();

		for (int j = 0; j != types->Length; ++j)
		{
			std::string qname;
			std::string name = ConvertToUnmanaged(types[j]->Name);
			std::string genericName;
			std::string genericQName;

			int p = name.find("`");
			if (p != std::string::npos)
			{
				genericName = name.substr(0, p);
			}

			if (types[j]->Namespace != nullptr)
			{
				std::string ns = ConvertToUnmanaged(types[j]->Namespace);
				qname = ns + "." + name;
				genericQName = ns + "." + genericName;
				Namespaces.insert(ns);

				p = ns.find(".");
				while (p != std::string::npos)
				{
					std::string subns = ns.substr(0, p);
					Namespaces.insert(subns);
					p = ns.find(".", p + 1);
				}
			}
			else
			{
				qname = name;
				genericQName = genericName;
			}

			auto t = Types.find(qname);
			if (t != Types.cend())
			{
				continue;
			}

			Types.insert(std::make_pair(qname, DynamicTypeHandle(types[j])));

			if (!genericName.empty())
			{
				Types.insert(std::make_pair(genericQName, DynamicTypeHandle(types[j])));
			}
		}
	}

} // namespace InteropPython
