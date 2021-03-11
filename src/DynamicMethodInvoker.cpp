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

	System::Delegate ^ DynamicMethodInvoker::GetDelegate() const
	{
		PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Creating action...");
		array<ParameterInfo ^> ^pis = _mi->GetParameters();
		bool isFunc = !_mi->ReturnType->Equals(System::Void::typeid);
		int nArgs = isFunc ? pis->Length + 1 : pis->Length;
		System::Object ^obj = _obj;
		System::Delegate ^del;
		MethodInfo ^mi = _mi;
		System::Type ^at;

		if (nArgs == 0)
		{
			at = System::Action::typeid;
		}
		else
		{
			std::string dt = 
				(isFunc ? std::string("System.Func") : std::string("System.Action")) +
				std::string("`") + 
				boost::lexical_cast<std::string>(nArgs);

			auto pt = DynamicTypesCache::GetInstance().Types.find(dt);
			if (pt == DynamicTypesCache::GetInstance().Types.cend())
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG((std::string("No such type: ") + dt));
				return nullptr;
			}
			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG((std::string("Using: ") + dt));
			at = pt->second.GetTypeObject();

			// Need to specialize Action/Func with proper type args
			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Adding argument types");
			auto tas = gcnew array<System::Type ^>(nArgs);
			for (int i = 0; i != pis->Length; ++i)
			{
				tas[i] = pis[i]->ParameterType;
			}
			if (isFunc)
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Adding return type");
				tas[nArgs - 1] = _mi->ReturnType;
			}
			at = at->GetGenericTypeDefinition()->MakeGenericType(tas);
		}

		try
		{
			if (obj == nullptr)
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Creating static delegate");
				del = System::Delegate::CreateDelegate(at, mi);
			}
			else
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Creating member delegate");
				del = System::Delegate::CreateDelegate(at, obj, mi);
			}
			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Delegate created");
			return del;
		}
		PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
	}

} // namespace InteropPython
