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

	ref class ActionProxy
	{
	public:
		void Invoke()
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			obj();
		}

		generic<class A1> void Invoke(A1 a1)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			obj(x1);
		}

		generic<class A1, class A2> void Invoke(A1 a1, A2 a2)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			obj(x1, x2);
		}

		generic<class A1, class A2, class A3> void Invoke(A1 a1, A2 a2, A3 a3)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			obj(x1, x2, x3);
		}

		generic<class A1, class A2, class A3, class A4> void Invoke(A1 a1, A2 a2, A3 a3, A4 a4)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			boost::python::object x4 = DynamicObjectDetail::ConvertToPython(a4);
			obj(x1, x2, x3, x4);
		}

		generic<class A1, class A2, class A3, class A4, class A5> void Invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			boost::python::object x4 = DynamicObjectDetail::ConvertToPython(a4);
			boost::python::object x5 = DynamicObjectDetail::ConvertToPython(a5);
			obj(x1, x2, x3, x4, x5);
		}

		generic<class A1, class A2, class A3, class A4, class A5, class A6> void Invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			boost::python::object x4 = DynamicObjectDetail::ConvertToPython(a4);
			boost::python::object x5 = DynamicObjectDetail::ConvertToPython(a5);
			boost::python::object x6 = DynamicObjectDetail::ConvertToPython(a6);
			obj(x1, x2, x3, x4, x5, x6);
		}

		generic<class A1, class A2, class A3, class A4, class A5, class A6, class A7> void Invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			boost::python::object x4 = DynamicObjectDetail::ConvertToPython(a4);
			boost::python::object x5 = DynamicObjectDetail::ConvertToPython(a5);
			boost::python::object x6 = DynamicObjectDetail::ConvertToPython(a6);
			boost::python::object x7 = DynamicObjectDetail::ConvertToPython(a7);
			obj(x1, x2, x3, x4, x5, x6, x7);
		}

		generic<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8> void Invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			boost::python::object x4 = DynamicObjectDetail::ConvertToPython(a4);
			boost::python::object x5 = DynamicObjectDetail::ConvertToPython(a5);
			boost::python::object x6 = DynamicObjectDetail::ConvertToPython(a6);
			boost::python::object x7 = DynamicObjectDetail::ConvertToPython(a7);
			boost::python::object x8 = DynamicObjectDetail::ConvertToPython(a8);
			obj(x1, x2, x3, x4, x5, x6, x7, x8);
		}

		generic<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9> void Invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			boost::python::object x4 = DynamicObjectDetail::ConvertToPython(a4);
			boost::python::object x5 = DynamicObjectDetail::ConvertToPython(a5);
			boost::python::object x6 = DynamicObjectDetail::ConvertToPython(a6);
			boost::python::object x7 = DynamicObjectDetail::ConvertToPython(a7);
			boost::python::object x8 = DynamicObjectDetail::ConvertToPython(a8);
			boost::python::object x9 = DynamicObjectDetail::ConvertToPython(a9);
			obj(x1, x2, x3, x4, x5, x6, x7, x8, x9);
		}

		generic<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10> void Invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			boost::python::object x4 = DynamicObjectDetail::ConvertToPython(a4);
			boost::python::object x5 = DynamicObjectDetail::ConvertToPython(a5);
			boost::python::object x6 = DynamicObjectDetail::ConvertToPython(a6);
			boost::python::object x7 = DynamicObjectDetail::ConvertToPython(a7);
			boost::python::object x8 = DynamicObjectDetail::ConvertToPython(a8);
			boost::python::object x9 = DynamicObjectDetail::ConvertToPython(a9);
			boost::python::object x10 = DynamicObjectDetail::ConvertToPython(a10);
			obj(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10);
		}

		ActionProxy(boost::python::object obj)
		{
			_obj = obj.ptr();
			Py_XINCREF(_obj);
		}

		~ActionProxy()
		{
			Py_XDECREF(_obj);
		}

		PyObject *_obj;
	};

	ref class FuncProxy
	{
	public:

		generic<class R> R Invoke()
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object rv = obj();
			return GetReturnValue<R>(rv);
		}

		generic<class A1, class R> R Invoke(A1 a1)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object rv = obj(x1);
			return GetReturnValue<R>(rv);
		}

		generic<class A1, class A2, class R> R Invoke(A1 a1, A2 a2)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object rv = obj(x1, x2);
			return GetReturnValue<R>(rv);
		}

		generic<class A1, class A2, class A3, class R> R Invoke(A1 a1, A2 a2, A3 a3)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			boost::python::object rv = obj(x1, x2, x3);
			return GetReturnValue<R>(rv);
		}

		generic<class A1, class A2, class A3, class A4, class R> R Invoke(A1 a1, A2 a2, A3 a3, A4 a4)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			boost::python::object x4 = DynamicObjectDetail::ConvertToPython(a4);
			boost::python::object rv = obj(x1, x2, x3, x4);
			return GetReturnValue<R>(rv);
		}

		generic<class A1, class A2, class A3, class A4, class A5, class R> R Invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			boost::python::object x4 = DynamicObjectDetail::ConvertToPython(a4);
			boost::python::object x5 = DynamicObjectDetail::ConvertToPython(a5);
			boost::python::object rv = obj(x1, x2, x3, x4, x5);
			return GetReturnValue<R>(rv);
		}

		generic<class A1, class A2, class A3, class A4, class A5, class A6, class R> R Invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			boost::python::object x4 = DynamicObjectDetail::ConvertToPython(a4);
			boost::python::object x5 = DynamicObjectDetail::ConvertToPython(a5);
			boost::python::object x6 = DynamicObjectDetail::ConvertToPython(a6);
			boost::python::object rv = obj(x1, x2, x3, x4, x5, x6);
			return GetReturnValue<R>(rv);
		}

		generic<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class R> R Invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			boost::python::object x4 = DynamicObjectDetail::ConvertToPython(a4);
			boost::python::object x5 = DynamicObjectDetail::ConvertToPython(a5);
			boost::python::object x6 = DynamicObjectDetail::ConvertToPython(a6);
			boost::python::object x7 = DynamicObjectDetail::ConvertToPython(a7);
			boost::python::object rv = obj(x1, x2, x3, x4, x5, x6, x7);
			return GetReturnValue<R>(rv);
		}

		generic<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class R> R Invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			boost::python::object x4 = DynamicObjectDetail::ConvertToPython(a4);
			boost::python::object x5 = DynamicObjectDetail::ConvertToPython(a5);
			boost::python::object x6 = DynamicObjectDetail::ConvertToPython(a6);
			boost::python::object x7 = DynamicObjectDetail::ConvertToPython(a7);
			boost::python::object x8 = DynamicObjectDetail::ConvertToPython(a8);
			boost::python::object rv = obj(x1, x2, x3, x4, x5, x6, x7, x8);
			return GetReturnValue<R>(rv);
		}

		generic<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class R> R Invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			boost::python::object x4 = DynamicObjectDetail::ConvertToPython(a4);
			boost::python::object x5 = DynamicObjectDetail::ConvertToPython(a5);
			boost::python::object x6 = DynamicObjectDetail::ConvertToPython(a6);
			boost::python::object x7 = DynamicObjectDetail::ConvertToPython(a7);
			boost::python::object x8 = DynamicObjectDetail::ConvertToPython(a8);
			boost::python::object x9 = DynamicObjectDetail::ConvertToPython(a9);
			boost::python::object rv = obj(x1, x2, x3, x4, x5, x6, x7, x8, x9);
			return GetReturnValue<R>(rv);
		}

		generic<class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10, class R> R Invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10)
		{
			AcquireGIL lk;
			boost::python::object obj(boost::python::borrowed(_obj));
			boost::python::object x1 = DynamicObjectDetail::ConvertToPython(a1);
			boost::python::object x2 = DynamicObjectDetail::ConvertToPython(a2);
			boost::python::object x3 = DynamicObjectDetail::ConvertToPython(a3);
			boost::python::object x4 = DynamicObjectDetail::ConvertToPython(a4);
			boost::python::object x5 = DynamicObjectDetail::ConvertToPython(a5);
			boost::python::object x6 = DynamicObjectDetail::ConvertToPython(a6);
			boost::python::object x7 = DynamicObjectDetail::ConvertToPython(a7);
			boost::python::object x8 = DynamicObjectDetail::ConvertToPython(a8);
			boost::python::object x9 = DynamicObjectDetail::ConvertToPython(a9);
			boost::python::object x10 = DynamicObjectDetail::ConvertToPython(a10);
			boost::python::object rv = obj(x1, x2, x3, x4, x5, x6, x7, x8, x9, x10);
			return GetReturnValue<R>(rv);
		}

		generic<class R> R GetReturnValue(boost::python::object val)
		{
			System::Object ^obj = DynamicObjectDetail::ConvertToManaged(val, _returnType);
			return safe_cast<R>(obj);
		}

		FuncProxy(boost::python::object obj, System::Type ^returnType) : _returnType(returnType)
		{
			_obj = obj.ptr();
			Py_XINCREF(_obj);
		}

		~FuncProxy()
		{
			Py_XDECREF(_obj);
		}

		PyObject *_obj;
		System::Type ^_returnType;
	};

	System::Object ^ ConvertToManagedObject(const boost::python::object &fromValue, System::Type ^resultType)
	{
		PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("Converting to managed object", fromValue);

		if (fromValue.is_none()) 
		{
			// TODO: Currently not working, since None causes Boost.Python to think that we pass less parameters than expected.
			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Type conversion to nullptr");
			return nullptr;
		}

		boost::python::extract<const DynamicObjectHandle &> maybeHandle(fromValue);
		if (!maybeHandle.check() && PyObject_HasAttrString(fromValue.ptr(), "__object__"))
      {
         boost::python::object obj = fromValue.attr("__object__");
		   maybeHandle = boost::python::extract<const DynamicObjectHandle &>(obj);
      }
		if (maybeHandle.check())
		{
			const DynamicObjectHandle &handle = maybeHandle;

			auto toValue = handle.GetObject();
			if (toValue == nullptr)
			{
				toValue = handle.GetTypeObject();
			}

			if (resultType->IsAssignableFrom(toValue->GetType()))
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("No type conversion required");
				return toValue;
			}

			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Type conversion unsupported");
		}

		if (resultType->IsAssignableFrom(System::Object::typeid))
		{
			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Converting to System::Object");

			boost::python::extract<int> maybeLong(fromValue);
			if (maybeLong.check())
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Converting to System::Int32");
				long lv = maybeLong;
				return (Int32)lv;
			}

			boost::python::extract<double> maybeDouble(fromValue);
			if (maybeDouble.check())
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Converting to System::Double");
				double lv = maybeDouble;
				return (Double)lv;
			}

			boost::python::extract<std::string> maybeString(fromValue);
			if (maybeString.check())
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Converting to System::String");
				return ConvertToManagedString(maybeString);
			}

			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Type conversion unsupported");
		}

		if (System::Delegate::typeid->IsAssignableFrom(resultType))
		{
			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Converting to callback...");
			boost::python::extract<const DynamicMethodInvoker &> maybeMethod(fromValue);
			if (maybeMethod.check())
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Using extracted method...");
				const DynamicMethodInvoker &method = maybeMethod;
				System::Delegate ^del = method.GetDelegate();
				return del;
			}
			else if (resultType->Name->StartsWith("Action"))
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Building action proxy...");

				auto methods = ActionProxy::typeid->GetMethods();
				array<System::Type ^> ^argTypes = resultType->GetGenericArguments();
				
				System::Reflection::MethodInfo ^mi;

				if (!resultType->IsGenericType)
				{
					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Building parameter-less action proxy...");

					for (int i = 0; i != methods->Length; ++i)
					{
						mi = methods[i];

						if (!mi->Name->Equals("Invoke"))
						{
							continue;
						}

						if (mi->GetParameters()->Length != 0)
						{
							continue;
						}

						ActionProxy ^proxy = gcnew ActionProxy(fromValue);
						System::Delegate ^del = System::Delegate::CreateDelegate(resultType, proxy, mi);
						return del;
					}
				}

				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Building action proxy w/ parameters...");

				for (int i = 0; i != methods->Length; ++i)
				{
					mi = methods[i];

					if (!mi->Name->Equals("Invoke"))
					{
						continue;
					}

					if (mi->GetGenericArguments()->Length != argTypes->Length)
					{
						continue;
					}

					mi = mi->MakeGenericMethod(argTypes);
					ActionProxy ^proxy = gcnew ActionProxy(fromValue);
					System::Delegate ^del = System::Delegate::CreateDelegate(resultType, proxy, mi);
					return del;
				}
			}
			else if (resultType->Name->StartsWith("Func"))
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Building func proxy...");

				auto methods = FuncProxy::typeid->GetMethods();
				array<System::Type ^> ^argTypes = resultType->GetGenericArguments();

				System::Reflection::MethodInfo ^mi;

				for (int i = 0; i != methods->Length; ++i)
				{
					mi = methods[i];

					if (!mi->Name->Equals("Invoke"))
					{
						continue;
					}

					if (mi->GetGenericArguments()->Length != argTypes->Length)
					{
						continue;
					}

					mi = mi->MakeGenericMethod(argTypes);
					FuncProxy ^proxy = gcnew FuncProxy(fromValue, mi->ReturnType);
					System::Delegate ^del = System::Delegate::CreateDelegate(resultType, proxy, mi);
					return del;
				}
			}
			else
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Trying arbitrary delegate...");

				System::Reflection::MethodInfo ^mi0 = resultType->GetMethod("Invoke");
				array<System::Reflection::ParameterInfo ^> ^parameters = mi0->GetParameters();

				if (mi0->ReturnType->Equals(System::Void::typeid))
				{
					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Building action proxy...");

					auto methods = ActionProxy::typeid->GetMethods();
					System::Reflection::MethodInfo ^mi;

					if (parameters->Length == 0)
					{
						PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Building parameter-less action proxy...");

						for (int i = 0; i != methods->Length; ++i)
						{
							mi = methods[i];

							if (!mi->Name->Equals("Invoke"))
							{
								continue;
							}

							if (mi->GetParameters()->Length != 0)
							{
								continue;
							}

							ActionProxy ^proxy = gcnew ActionProxy(fromValue);
							System::Delegate ^del = System::Delegate::CreateDelegate(resultType, proxy, mi);
							return del;
						}
					}

					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Building action proxy w/ parameters...");

					array<System::Type ^> ^argTypes = gcnew array<System::Type ^>(parameters->Length);

					for (int i = 0; i != parameters->Length; ++i)
					{
						argTypes[i] = parameters[i]->ParameterType;
					}

					for (int i = 0; i != methods->Length; ++i)
					{
						mi = methods[i];

						if (!mi->Name->Equals("Invoke"))
						{
							continue;
						}

						if (mi->GetGenericArguments()->Length != argTypes->Length)
						{
							continue;
						}

						mi = mi->MakeGenericMethod(argTypes);
						ActionProxy ^proxy = gcnew ActionProxy(fromValue);
						System::Delegate ^del = System::Delegate::CreateDelegate(resultType, proxy, mi);
						return del;
					}
				}
				else
				{
					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Building func proxy...");

					auto methods = FuncProxy::typeid->GetMethods();
					System::Reflection::MethodInfo ^mi;

					array<System::Type ^> ^argTypes = gcnew array<System::Type ^>(parameters->Length + 1);

					for (int i = 0; i != parameters->Length; ++i)
					{
						argTypes[i] = parameters[i]->ParameterType;
					}

					argTypes[argTypes->Length - 1] = mi0->ReturnType;

					for (int i = 0; i != methods->Length; ++i)
					{
						mi = methods[i];

						if (!mi->Name->Equals("Invoke"))
						{
							continue;
						}

						if (mi->GetGenericArguments()->Length != argTypes->Length)
						{
							continue;
						}

						mi = mi->MakeGenericMethod(argTypes);
						FuncProxy ^proxy = gcnew FuncProxy(fromValue, mi->ReturnType);
						System::Delegate ^del = System::Delegate::CreateDelegate(resultType, proxy, mi);
						return del;
					}
				}
			}
		}

		boost::python::extract<boost::python::list> maybeList(fromValue);
		if (maybeList.check())
		{
			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Converting list...");

			if (resultType->IsArray)
			{
				const int n = boost::python::len(fromValue);

				auto elementType = resultType->GetElementType();
				auto arrayDim = gcnew array<int>(1);
				arrayDim[0] = n;
				System::Array ^result = System::Array::CreateInstance(elementType, arrayDim);

				for (int i = 0; i != n; ++i)
				{
					boost::python::object item = fromValue[i];

					PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("Converting item", item);
					System::Object ^value = DynamicObjectDetail::ConvertToManaged(item, elementType);

					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Setting item...");
					result->SetValue(value, i);
				}

				return result;
			}
			else if (resultType->IsGenericType)
			{
				array<System::Type ^> ^genericArgs = resultType->GetGenericArguments();
				if (genericArgs->Length == 1)
				{
					if (resultType->IsInterface)
					{
						PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Target is an interface, using List implementation...");
						resultType = List<System::Object ^>::typeid->
							GetGenericTypeDefinition()->MakeGenericType(genericArgs);
					}

					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Constructing...")
						auto ci = resultType->GetConstructor(gcnew array<System::Type ^>(0));
					auto instance = ci->Invoke(gcnew array<System::Object ^>(0));

					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Getting Add method...")
						auto argTypes = gcnew array<System::Type ^>(1);
					argTypes[0] = genericArgs[0];
					auto mi = resultType->GetMethod("Add", argTypes);

					const int n = boost::python::len(fromValue);
					auto args = gcnew array<System::Object ^>(1);

					for (int i = 0; i != n; ++i)
					{
						boost::python::object item = fromValue[i];

						PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("Converting item", item);
						System::Object ^value = DynamicObjectDetail::ConvertToManaged(item, genericArgs[0]);

						PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Adding item...");
						args[0] = value;
						mi->Invoke(instance, args);
					}

					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Returning result...");
					return instance;
				}
			}
		}

		boost::python::extract<boost::python::dict> maybeDict(fromValue);
		if (maybeDict.check())
		{
			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Converting dict...");

			if (resultType->IsGenericType)
			{
				array<System::Type ^> ^genericArgs = resultType->GetGenericArguments();
				if (genericArgs->Length == 2)
				{
					if (resultType->IsInterface)
					{
						PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Target is an interface, using Dictionary implementation...");
						resultType = Dictionary<System::Object ^, System::Object ^>::typeid->
							GetGenericTypeDefinition()->MakeGenericType(genericArgs);
					}

					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Constructing...")
						auto ci = resultType->GetConstructor(gcnew array<System::Type ^>(0));
					auto instance = ci->Invoke(gcnew array<System::Object ^>(0));

					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Getting Add method...")
						auto argTypes = gcnew array<System::Type ^>(2);
					argTypes[0] = genericArgs[0];
					argTypes[1] = genericArgs[1];
					auto mi = resultType->GetMethod("Add", argTypes);

					boost::python::dict d = maybeDict;
					boost::python::list items = d.items();

					const int n = boost::python::len(items);
					auto args = gcnew array<System::Object ^>(2);

					for (int i = 0; i != n; ++i)
					{
						boost::python::tuple pair = boost::python::extract<boost::python::tuple>(items[i]);

						PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("Converting item", pair);
						System::Object ^key = DynamicObjectDetail::ConvertToManaged(pair[0], genericArgs[0]);
						System::Object ^value = DynamicObjectDetail::ConvertToManaged(pair[1], genericArgs[1]);

						PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Adding item...");
						args[0] = key;
						args[1] = value;
						mi->Invoke(instance, args);
					}

					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Returning result...");
					return instance;
				}
			}
		}

		PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Type conversion unsupported");
		throw_invalid_cast();
		throw std::runtime_error("Invalid cast");
	}

}//namespace InteropPython
