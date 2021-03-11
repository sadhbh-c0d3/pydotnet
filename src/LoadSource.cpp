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

	void DynamicAppDomain::LoadSource(const char *sourceCode, const std::string &outputFile, boost::python::object assemblies, const std::string &compilerOptions)
	{
		try
		{
			using namespace Microsoft::CSharp;
			using namespace System::CodeDom::Compiler;
			static int counter = 0;

			System::String ^source = ConvertToManagedString(sourceCode);

			System::String ^outputPath = ConvertToManagedString(outputFile);

			if (System::IO::File::Exists(outputPath)) 
			{
				System::IO::File::Delete(outputPath);
			}

			auto parameters = gcnew CompilerParameters(gcnew array<System::String ^>(0), outputPath);

			parameters->GenerateInMemory = true;
			parameters->ReferencedAssemblies->Add("System.dll");

            if (!compilerOptions.empty())
            {
                parameters->CompilerOptions = ConvertToManagedString(compilerOptions);
            }

			const int n = boost::python::len(assemblies);
			for (int i = 0; i != n; ++i)
			{
				std::string s = boost::python::extract<std::string>(assemblies[i]);
				parameters->ReferencedAssemblies->Add(ConvertToManagedString(s));
			}

			auto compiler = gcnew CSharpCodeProvider();

			auto result = compiler->CompileAssemblyFromSource(parameters, source);
			
			bool anyErrors = false;
			auto errors = result->Errors;
			if (errors != nullptr)
			{
				std::ostringstream ss;

				for (int i = 0; i != errors->Count; ++i)
				{
					if (!errors[i]->IsWarning)
					{
						if (anyErrors)
						{
							ss << std::endl;
						}

						// ERROR CS0116 (1,1): A namespace cannot directly contain members such as fields or methods

						ss	<< "ERROR " << ConvertToUnmanaged(errors[i]->ErrorNumber)
							<< " (" << errors[i]->Line 
							<< "," << errors[i]->Column 
							<< "): " << ConvertToUnmanaged(errors[i]->ErrorText);

						anyErrors = true;
					}
				}

				if (anyErrors)
				{
					throw std::runtime_error(ss.str());
				}
			}

			DynamicTypesCache::GetInstance().Refresh();
		}
		catch (std::runtime_error &err)
		{
			throw_exception(err.what());
			throw;
		}
		PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
	}

}
