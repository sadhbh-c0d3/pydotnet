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

#ifndef INCLUDED_PYDOTNET_DYNAMIC_OBJECT_HANDLE_H
#define INCLUDED_PYDOTNET_DYNAMIC_OBJECT_HANDLE_H

#include "InteropPythonTypes.h"
#include "ObjectHandle.h"
#include "InteropPythonExceptions.h"
#include "DynamicTypeConverterChoice.h"

//#define PYDOTNET_REGISTER_PRINT_DEBUG(TEXT)
#define PYDOTNET_REGISTER_PRINT_DEBUG(TEXT) if (g_DebugModuleInit) PYDOTNET_PRINT_DEBUG(TEXT)

// #define PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG(TEXT)
#define PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG(TEXT) if (g_DebugDynamicInvokes) { PYDOTNET_PRINT_DEBUG(TEXT); }

//#define PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG(TEXT, ARGS)
#define PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG(TEXT, ARGS) \
	if (g_DebugDynamicInvokes) \
	{ PYDOTNET_PRINT_DEBUG((std::string(TEXT) + " with parameters: " + \
	std::string(boost::python::extract<std::string>( \
	boost::python::object( \
	boost::python::detail::new_reference( \
	PyObject_Repr(boost::python::object(ARGS).ptr())\
	)))))); }



#if PY_VERSION_HEX >= 0x03000000
#define PYDOTNET_DEF_ITERATOR_NEXT(NEXTMETHOD) def("__next__", NEXTMETHOD)
#else
#define PYDOTNET_DEF_ITERATOR_NEXT(NEXTMETHOD) def("next", NEXTMETHOD)
#endif

namespace InteropPython {

	//////
	//
	// Wrapper Classes
	//

	struct DynamicObjectDetail
	{
		typedef System::Reflection::BindingFlags BindingFlags;
		typedef System::Reflection::MemberInfo MemberInfo;
		typedef System::Reflection::PropertyInfo PropertyInfo;
		typedef System::Reflection::FieldInfo FieldInfo;
		typedef System::Reflection::MethodBase MethodBase;
		typedef System::Reflection::MethodInfo MethodInfo;
		typedef System::Reflection::ConstructorInfo ConstructorInfo;
		typedef System::Reflection::ParameterInfo ParameterInfo;

	private:
		struct ConversionFromManaged
		{
			gcroot<System::Object ^> value;
			boost::python::object result;

			ConversionFromManaged(System::Object ^value_) : value(value_)
			{}

			template<typename ManagedTypeConversionInfo> struct Action
			{
				typedef typename ManagedTypeConversionInfo::ManagedType FromType;
				typedef typename ManagedTypeConversionInfo::UnmanagedType ToType;

				static void Apply(ConversionFromManaged &args)
				{
					args.result = boost::python::object(FromManagedTypeConverter<FromType, ToType>::Convert(args.value));
				}
			};
		};

		struct ConversionToManaged
		{
			boost::python::object value;
			gcroot<System::Object ^> result;
			gcroot<System::Type ^> resultType;

			ConversionToManaged(boost::python::object value_, System::Type ^resultType_) 
				: value(value_), resultType(resultType_)
			{}

			template<typename ManagedTypeConversionInfo> struct Action
			{
				typedef typename ManagedTypeConversionInfo::UnmanagedType FromType;
				typedef typename ManagedTypeConversionInfo::ManagedType ToType;

				static void Apply(ConversionToManaged &args)
				{
					boost::python::extract<FromType> get_value(args.value);
					if (!get_value.check())
					{
						throw_invalid_cast();
						throw std::runtime_error("Invalid cast");
					}				
					FromType val = get_value;
					args.result = ToManagedTypeConverter<FromType, ToType>::Convert(val, args.resultType);
				}
			};

			// Specialization(s)
			template<> struct Action<ManagedTypeConversionInfo<System::Object ^, boost::python::object> >;
		};

	public:
		static boost::python::object ConvertToPython(System::Object ^obj, System::Type ^typ)
		{
			ConversionFromManaged conversion(obj);
			DynamicTypeConverterChoice::Apply<ConversionFromManaged::Action>(obj->GetType(), conversion);
			return conversion.result;
		}

		static boost::python::object ConvertToPython(System::Object ^obj)
		{
			if (obj == nullptr)
			{
				return boost::python::object();
			}

			return ConvertToPython(obj, obj->GetType());
		}

		static System::Object ^ConvertToManaged(boost::python::object val, System::Type ^typ)
		{
			ConversionToManaged conversion(val, typ);
			DynamicTypeConverterChoice::Apply<ConversionToManaged::Action>(typ, conversion);
			return conversion.result;
		}

		static bool IsConstantProperty(MemberInfo ^mi)
		{
			return (mi->MemberType == System::Reflection::MemberTypes::Method) ||
				(mi->MemberType == System::Reflection::MemberTypes::NestedType);
		}

		static boost::python::object DoGetProperty(System::Object ^obj, PropertyInfo ^pi);

		static boost::python::object DoGetProperty(System::Object ^obj, FieldInfo ^pi);

		static boost::python::object DoGetProperty(System::Object ^obj, MethodInfo ^pi);

		static boost::python::object DoGetProperty(System::Object ^obj, System::Type ^pi);

		static boost::python::object DoGetProperty(System::Object ^obj, MemberInfo ^mi)
		{
			if (mi->MemberType == System::Reflection::MemberTypes::Property)
			{
				return DoGetProperty(obj, safe_cast<PropertyInfo^>(mi));
			}
			else if (mi->MemberType == System::Reflection::MemberTypes::Field)
			{
				return DoGetProperty(obj, safe_cast<FieldInfo^>(mi));
			}
			else if (mi->MemberType == System::Reflection::MemberTypes::Method)
			{
				return DoGetProperty(obj, safe_cast<MethodInfo^>(mi));
			}
			else if (mi->MemberType == System::Reflection::MemberTypes::NestedType)
			{
				return DoGetProperty(obj, safe_cast<System::Type^>(mi));
			}
			else 
			{
				std::string txt = "Unknown property type: " + ConvertToUnmanaged(mi->MemberType.ToString());
				PYDOTNET_REGISTER_PRINT_DEBUG(txt);
			}
			return boost::python::object();
		}

		static boost::python::object DoGetProperty(System::Object ^obj, array<MemberInfo ^> ^mi);

		static bool DoSetProperty(System::Object ^obj, PropertyInfo ^pi, boost::python::object val)
		{
			System::Object ^value = ConvertToManaged(val, pi->PropertyType);

			if (value == nullptr)
			{
				return false;
			}

			{
				ReleaseGIL lk;
				pi->SetValue(obj, value, nullptr);
			}
			return true;
		}

		static bool DoSetProperty(System::Object ^obj, FieldInfo ^pi, boost::python::object val)
		{
			System::Object ^value = ConvertToManaged(val, pi->FieldType);

			if (value == nullptr)
			{
				return false;
			}

			pi->SetValue(obj, value);
			return true;
		}

		static bool DoSetProperty(System::Object ^obj, MemberInfo ^mi, boost::python::object val)
		{
			if (mi->MemberType == System::Reflection::MemberTypes::Property)
			{
				return DoSetProperty(obj, safe_cast<PropertyInfo^>(mi), val);
			}
			else if (mi->MemberType == System::Reflection::MemberTypes::Field)
			{
				return DoSetProperty(obj, safe_cast<FieldInfo^>(mi), val);
			}
			return false;
		}

		static System::Type ^GetPropertyType(MemberInfo ^mi)
		{
			if (mi->MemberType == System::Reflection::MemberTypes::Property)
			{
				return safe_cast<PropertyInfo^>(mi)->PropertyType;
			}
			else if (mi->MemberType == System::Reflection::MemberTypes::Field)
			{
				return safe_cast<FieldInfo^>(mi)->FieldType;
			}
			return nullptr;
		}
	};

	struct DynamicObjectHandle : ObjectHandle, protected DynamicObjectDetail
	{
		DynamicObjectHandle(System::Object ^obj, System::Type ^typ) : ObjectHandle(obj), _typ(typ)
		{
			Init();
		}

		DynamicObjectHandle(System::Object ^obj) : ObjectHandle(obj)
		{
			Init();
		}

		explicit DynamicObjectHandle(const ObjectHandle &handle) : ObjectHandle(handle)
		{
			Init();
		}

		void Init();

		boost::python::object GetProperty(const std::string &name);

		void SetProperty(const char *name, boost::python::object val);

		boost::python::list GetProperties();

		boost::python::object GetItem(boost::python::object key);

		void SetItem(boost::python::object key, boost::python::object val);

		int GetLength();

		bool IsStatic()
		{
			return _flags.HasFlag(BindingFlags::Static);
		}

		int GetHashCode() const
		{
			if (GetObject() == nullptr)
			{
				if (GetTypeObject() == nullptr)
				{ 
					return 0;
				}

				return _typ->GetHashCode();
			}

			return ObjectHandle::GetHashCode();
		}

		boost::python::object GetIter();

		boost::python::str ToString()
		{
			// Used when calling: str(x), print(x)
			System::Object ^obj = GetObject();

			if (obj == nullptr)
			{
				if (GetTypeObject() == nullptr)
				{
					return boost::python::str();
				}

				return boost::python::str(ConvertToUnmanaged(_typ->ToString()));
			}

			return boost::python::str(ConvertToUnmanaged(obj->ToString()));
		}

		boost::python::str ToReprString();

		boost::python::str ToPrettyString();

		System::Type ^GetTypeObject() const
		{
			return _typ;
		}

		boost::python::object GetTypeId() const
		{
			if (GetTypeObject() == nullptr)
			{
				return boost::python::object();
			}

			return boost::python::object(DynamicObjectHandle(_typ));
		}

		boost::python::object GetClassId() const;

		static boost::python::object GetGetAttrHook()
		{
			return _getAttrHook;
		}

		static void SetGetAttrHook(boost::python::object hook);

		boost::python::object GetAttr(const std::string &name);

		static void Register(const std::string &name)
		{
			using namespace boost::python;

			PYDOTNET_REGISTER_PRINT_DEBUG(name);

			class_<DynamicObjectHandle, bases<ObjectHandle>>(name.c_str(), init<const ObjectHandle &>())
				.add_property("__typeid__", &DynamicObjectHandle::GetTypeId, "Allows access to managed type information")
				.add_property("__classid__", &DynamicObjectHandle::GetClassId, "Allows access to static properties and methods")
				.add_static_property("__getattrhook__", &DynamicObjectHandle::GetGetAttrHook, &DynamicObjectHandle::SetGetAttrHook)
				.def("__getattr__", &DynamicObjectHandle::GetAttr, "Gets field, property, method, method overloads or nested type from managed type")
				.def("__setattr__", &DynamicObjectHandle::SetProperty, "Sets field or property of managed type")
				.def("__getitem__", &DynamicObjectHandle::GetItem, "Gets item from array T[] or list IList<T>")
				.def("__setitem__", &DynamicObjectHandle::SetItem, "Sets item in array T[] or list IList<T>")
				.def("__len__", &DynamicObjectHandle::GetLength, "Gets length of array T[] or list IList<T>")
				.def("__iter__", &DynamicObjectHandle::GetIter, "Iterates over IEnumerable<T>")
				.def("__dir__", &DynamicObjectHandle::GetProperties, "Gets all available members of managed type")
				.def("__str__", &DynamicObjectHandle::ToString, "Formats string")
				.def("__repr__", &DynamicObjectHandle::ToReprString, "Formats simple representation string")
				.def("__pretty__", &DynamicObjectHandle::ToPrettyString, "Formats pretty representation string")
				;
		}

	private:
		gcroot<System::Type ^> _typ;
		BindingFlags _flags;
		std::map<std::string, boost::python::object> _cache;
		static boost::python::object _getAttrHook;
		static boost::python::object _getAttrBase;
	};

	struct DynamicIterator : private DynamicObjectDetail
	{
		DynamicIterator(System::Collections::IEnumerator ^iter) : _iter(iter)
		{}

		boost::python::object GetNext()
		{
			if (!_iter->MoveNext())
				throw_stop_iteration();

			System::Object ^value = _iter->Current;
			return ConvertToPython(value);
		}

		static void Register(const std::string &name)
		{
			using namespace boost::python;

			PYDOTNET_REGISTER_PRINT_DEBUG(name);

			class_<DynamicIterator>(name.c_str(), no_init)
				.PYDOTNET_DEF_ITERATOR_NEXT(&DynamicIterator::GetNext)
				;
		}

	private:
		gcroot<System::Collections::IEnumerator ^> _iter;
	};

	struct InvocationForwarding
	{
#pragma region Invoke0, Invoke1, ...
		boost::python::object Invoke0() 
		{ 
			return Invoke(boost::python::tuple());
		}

		boost::python::object Invoke1(const boost::python::object &a1)
		{
			boost::python::list lst;
			lst.append(a1);
			return Invoke(boost::python::tuple(lst));
		}

		boost::python::object Invoke2(
			const boost::python::object &a1, 
			const boost::python::object &a2)
		{
			boost::python::list lst;
			lst.append(a1);
			lst.append(a2);
			return Invoke(boost::python::tuple(lst));
		}

		boost::python::object Invoke3(
			const boost::python::object &a1, 
			const boost::python::object &a2, 
			const boost::python::object &a3)
		{
			boost::python::list lst;
			lst.append(a1);
			lst.append(a2);
			lst.append(a3);
			return Invoke(boost::python::tuple(lst));
		}

		boost::python::object Invoke4(
			const boost::python::object &a1, 
			const boost::python::object &a2, 
			const boost::python::object &a3, 
			const boost::python::object &a4)
		{
			boost::python::list lst;
			lst.append(a1);
			lst.append(a2);
			lst.append(a3);
			lst.append(a4);
			return Invoke(boost::python::tuple(lst));
		}

		boost::python::object Invoke5(
			const boost::python::object &a1, 
			const boost::python::object &a2, 
			const boost::python::object &a3, 
			const boost::python::object &a4, 
			const boost::python::object &a5)
		{
			boost::python::list lst;
			lst.append(a1);
			lst.append(a2);
			lst.append(a3);
			lst.append(a4);
			lst.append(a5);
			return Invoke(boost::python::tuple(lst));
		}

		boost::python::object Invoke6(
			const boost::python::object &a1, 
			const boost::python::object &a2, 
			const boost::python::object &a3, 
			const boost::python::object &a4, 
			const boost::python::object &a5, 
			const boost::python::object &a6)
		{
			boost::python::list lst;
			lst.append(a1);
			lst.append(a2);
			lst.append(a3);
			lst.append(a4);
			lst.append(a5);
			lst.append(a6);
			return Invoke(boost::python::tuple(lst));
		}

		boost::python::object Invoke7(
			const boost::python::object &a1, 
			const boost::python::object &a2, 
			const boost::python::object &a3, 
			const boost::python::object &a4, 
			const boost::python::object &a5, 
			const boost::python::object &a6, 
			const boost::python::object &a7)
		{
			boost::python::list lst;
			lst.append(a1);
			lst.append(a2);
			lst.append(a3);
			lst.append(a4);
			lst.append(a5);
			lst.append(a6);
			lst.append(a7);
			return Invoke(boost::python::tuple(lst));
		}

		boost::python::object Invoke8(
			const boost::python::object &a1, 
			const boost::python::object &a2, 
			const boost::python::object &a3, 
			const boost::python::object &a4, 
			const boost::python::object &a5, 
			const boost::python::object &a6, 
			const boost::python::object &a7, 
			const boost::python::object &a8)
		{
			boost::python::list lst;
			lst.append(a1);
			lst.append(a2);
			lst.append(a3);
			lst.append(a4);
			lst.append(a5);
			lst.append(a6);
			lst.append(a7);
			lst.append(a8);
			return Invoke(boost::python::tuple(lst));
		}

		boost::python::object Invoke9(
			const boost::python::object &a1, 
			const boost::python::object &a2, 
			const boost::python::object &a3, 
			const boost::python::object &a4, 
			const boost::python::object &a5, 
			const boost::python::object &a6, 
			const boost::python::object &a7, 
			const boost::python::object &a8, 
			const boost::python::object &a9)
		{
			boost::python::list lst;
			lst.append(a1);
			lst.append(a2);
			lst.append(a3);
			lst.append(a4);
			lst.append(a5);
			lst.append(a6);
			lst.append(a7);
			lst.append(a8);
			lst.append(a9);
			return Invoke(boost::python::tuple(lst));
		}

		boost::python::object Invoke10(
			const boost::python::object &a1, 
			const boost::python::object &a2, 
			const boost::python::object &a3, 
			const boost::python::object &a4, 
			const boost::python::object &a5, 
			const boost::python::object &a6, 
			const boost::python::object &a7, 
			const boost::python::object &a8, 
			const boost::python::object &a9, 
			const boost::python::object &a10)
		{
			boost::python::list lst;
			lst.append(a1);
			lst.append(a2);
			lst.append(a3);
			lst.append(a4);
			lst.append(a5);
			lst.append(a6);
			lst.append(a7);
			lst.append(a8);
			lst.append(a9);
			lst.append(a10);
			return Invoke(boost::python::tuple(lst));
		}
#pragma endregion

		virtual boost::python::object Invoke(const boost::python::tuple &args) = 0;

		static void Register(const std::string &name)
		{
			using namespace boost::python;

			PYDOTNET_REGISTER_PRINT_DEBUG(name);

			class_<InvocationForwarding, boost::noncopyable>(name.c_str(), no_init)
				.def("__invoke__", &InvocationForwarding::Invoke, "Calls directly with all arguments in one tuple") 
				.def("__call__", &InvocationForwarding::Invoke0)
				.def("__call__", &InvocationForwarding::Invoke1)
				.def("__call__", &InvocationForwarding::Invoke2)
				.def("__call__", &InvocationForwarding::Invoke3)
				.def("__call__", &InvocationForwarding::Invoke4)
				.def("__call__", &InvocationForwarding::Invoke5)
				.def("__call__", &InvocationForwarding::Invoke6)
				.def("__call__", &InvocationForwarding::Invoke7)
				.def("__call__", &InvocationForwarding::Invoke8)
				.def("__call__", &InvocationForwarding::Invoke9)
				.def("__call__", &InvocationForwarding::Invoke10)
				;
		}
	};

	struct DynamicCallable : protected DynamicObjectDetail, InvocationForwarding
	{
		DynamicCallable(System::Type ^rt, System::String ^name) : _rt(rt), _name(name)
		{}

		System::Object ^InvokeWithoutParameters()
		{
			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Invoking parameterless method...");
			try
			{
				return DoInvoke(gcnew array<System::Object ^>(0));
			}
			PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
		}

		System::Object ^InvokeWithParameters(const boost::python::tuple &args, array<ParameterInfo ^> ^pis)
		{
			try
			{
				int j = 0;
				int nArgs = len(args);
				int nOutArgs = 0;
				array<System::Object ^> ^params = gcnew array<System::Object ^>(pis->Length);

				for (int i = 0; i != pis->Length; ++i)
				{
					if (pis[i]->IsOut)
					{
						++nOutArgs;

						if (!pis[i]->IsIn)
						{
							params[i] = nullptr;
							continue;
						}
					}

					if (j == nArgs)
					{
						if (pis[i]->IsOptional)
						{
							params[i] = pis[i]->DefaultValue;
							continue;
						}

						throw_invalid_cast("Incorrect number of paramters");
						throw std::runtime_error("Incorrect number of paramters");
					}

					System::Type ^pt = pis[i]->ParameterType;
					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG(std::string("Parameter: ") + ConvertToUnmanaged(pt->Name));

					params[i] = ConvertToManaged(args[j], pis[i]->ParameterType);
					++j;
				}

				if (j < nArgs)
				{
					throw_invalid_cast("Incorrect number of paramters");
					throw std::runtime_error("Incorrect number of paramters");
				}

				PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("Invoking method", args);
				System::Object ^result = DoInvoke(params);

				if (nOutArgs == 0)
				{
					return result;
				}

				bool isVoid = _rt->Equals(System::Void::typeid);

				// If we have any OUT arguments, we return an array containing those followed by return value.
				array<System::Object ^> ^results = gcnew array<System::Object ^>(nOutArgs + (isVoid ? 0 : 1));

				int ri = 0;
				
				for (int i = 0; i != pis->Length; ++i)
				{
					if (pis[i]->IsOut)
					{
						results[ri] = params[i];
						++ri;
					}
				}
				
				if (!isVoid)
				{
					results[ri] = result;
				}

				return results;
			}
			PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
		}

		boost::python::object ConvertResultToPython(System::Object ^result)
		{
			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG(std::string("Method returns: ") + ConvertToUnmanaged(_rt->Name));

			if (result == nullptr || (_rt->Equals(System::Void::typeid) && !result->GetType()->IsArray))
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Method returned: None");
				return boost::python::object();
			}

			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Method returned...");
			return ConvertToPython(result, _rt);
		}

		boost::python::object Invoke(const boost::python::tuple &args)
		{
			using namespace boost::python;

			array<ParameterInfo ^> ^pis = GetParameters();

			System::Object ^result = (pis == nullptr || pis->Length == 0)
				? InvokeWithoutParameters()
				: InvokeWithParameters(args, pis);

			return ConvertResultToPython(result);
		}

		virtual array<ParameterInfo ^> ^GetParameters() const = 0;
		virtual System::Object ^DoInvoke(array<System::Object ^> ^args) = 0;
		
		virtual System::Object ^ GetCallableInfo() const = 0;
		virtual System::Type ^ GetDeclaringType() const = 0;
		virtual std::string GetCallableType() const = 0;
		virtual bool IsStatic() const = 0;
		
		System::Type ^ GetReturnType() const
		{
			return _rt;
		}

		std::string GetName() const
		{
			return ConvertToUnmanaged(_name.operator System::String ^());
		}

		std::string GetSignature() const
		{
			try{
				std::string signature = 
					(IsStatic() ? "static " : "") + GetName() + "(";

				array<ParameterInfo ^> ^pis = GetParameters();
				if (pis != nullptr)
				{
					const int n = pis->Length;
					for (int i = 0; i != n; ++i)
					{
						if (i > 0) 
							signature += ", ";

						signature += 
							ConvertToUnmanaged(pis[i]->ParameterType->Name) + 
							" " +
							ConvertToUnmanaged(pis[i]->Name);
					}
				}

				signature = signature + ")";

				if (_rt.operator System::Type ^() != nullptr)
				{
					signature += " -> " + ConvertToUnmanaged(_rt->Name);
				}

				return signature;
			}
			PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
		}

		boost::python::str ToString() const
		{
			auto declaringTypeName = std::string();
			auto declaringType = GetDeclaringType();

			if (declaringType != nullptr)
			{
				declaringTypeName = ConvertToUnmanaged(declaringType->Name) + ".";
			}

			return boost::python::str("<" + GetCallableType() + " " + declaringTypeName +  GetName() + ">");
		}

		boost::python::str ToReprString() const
		{
			return ToString();
		}

		boost::python::str ToPrettyString() const
		{
			return boost::python::str(GetSignature());
		}

		boost::python::object GetCallable() const
		{
			DynamicObjectHandle ci(GetCallableInfo());
			return boost::python::object(ci);
		}

		boost::python::object GetDynamicCallableInstance1(const boost::python::object &args) const;
		boost::python::object GetDynamicCallableInstance(const boost::python::tuple &args) const;

        virtual boost::python::object GetInstance() const = 0;
		virtual boost::python::object Clone() const = 0;

		static void Register(const std::string &name)
		{
			using namespace boost::python;

			PYDOTNET_REGISTER_PRINT_DEBUG(name);

			class_<DynamicCallable, bases<InvocationForwarding>, boost::noncopyable>(name.c_str(), no_init)
				.add_property("Name", &DynamicCallable::GetName, "Name")
				.add_property("__func__", &DynamicCallable::GetCallable)
				.add_property("__instance__", &DynamicCallable::GetInstance)
				.def("__str__", &DynamicCallable::ToString)
				.def("__repr__", &DynamicCallable::ToReprString)
				.def("__pretty__", &DynamicCallable::ToPrettyString)
				;
		}

	private:
		gcroot<System::Type ^> _rt;
		gcroot<System::String ^> _name;
	};

	struct DynamicCallableInstance : protected DynamicObjectDetail, InvocationForwarding
	{
		DynamicCallableInstance(boost::python::object callable, boost::python::tuple ptypes);

		boost::python::object Invoke(const boost::python::tuple &args)
		{
			try
			{
				if (len(args) != _ptypes.size())
				{
					throw_invalid_cast("Incorrect number of paramters");
					throw std::runtime_error("Incorrect number of paramters");
				}

				array<System::Object ^> ^params = gcnew array<System::Object ^>(_ptypes.size());

				for (int i = 0; i != params->Length; ++i)
				{
					System::Type ^pt = _ptypes[i];
					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG(std::string("Parameter: ") + ConvertToUnmanaged(pt->Name));

					params[i] = ConvertToManaged(args[i], pt);
				}

				PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("Invoking method", args);
				return DoInvoke(params);
			}
			PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
		}

		boost::python::object DoInvoke(array<System::Object ^> ^params)
		{
			try{
				boost::python::extract<DynamicCallable &> maybeCallable(_callable);
				DynamicCallable &callable = maybeCallable;
				System::Object ^result = callable.DoInvoke(params);
				return callable.ConvertResultToPython(result);
			}
			PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
		}
		
		std::string GetSignature() const
		{
			try{
				DynamicCallable &callable = GetDynamicCallable();

				std::string signature = 
					(callable.IsStatic() ? "static " : "") + GetName() + "(";

				array<ParameterInfo ^> ^pis = callable.GetParameters();
				const int n = pis->Length;
				for (int i = 0; i != n; ++i)
				{
					if (i > 0) 
						signature += ", ";

					signature += 
						ConvertToUnmanaged(pis[i]->ParameterType->Name) + 
						" " +
						ConvertToUnmanaged(_ptypes[i]->Name);
				}

				signature = signature + ")";

				if (callable.GetReturnType() != nullptr)
				{
					signature += " -> " + ConvertToUnmanaged(callable.GetReturnType()->Name);
				}

				return signature;
			}
			PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
		}

		std::string GetName() const
		{
			try{
				return GetDynamicCallable().GetName();
			}
			PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
		}

		boost::python::str ToString() const
		{
			try{
				return GetDynamicCallable().ToString();
			}
			PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
		}

		boost::python::str ToReprString() const
		{
			try{
				return GetDynamicCallable().ToReprString();
			}
			PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
		}

		boost::python::str ToPrettyString() const
		{
			return boost::python::str(GetSignature());
		}

		boost::python::object GetCallable() const
		{
			try{
				return GetDynamicCallable().GetCallable();
			}
			PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
		}

		boost::python::object GetInstance() const
		{
			try{
				return GetDynamicCallable().GetInstance();
			}
			PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
		}

		boost::python::list GetParameterTypes() const;

		DynamicCallable &GetDynamicCallable() const
		{
			boost::python::extract<DynamicCallable &> maybeCallable(_callable);
			return maybeCallable;
		}

		static void Register(const std::string &name)
		{
			using namespace boost::python;

			PYDOTNET_REGISTER_PRINT_DEBUG(name);

			class_<DynamicCallableInstance, bases<InvocationForwarding>>(name.c_str(), 
				init<boost::python::object, boost::python::tuple>())
				.add_property("Name", &DynamicCallableInstance::GetName, "Name")
				.add_property("__func__", &DynamicCallableInstance::GetCallable)
				.add_property("__paramtypes__", &DynamicCallableInstance::GetParameterTypes)
				.def("__str__", &DynamicCallableInstance::ToString)
				.def("__repr__", &DynamicCallableInstance::ToReprString)
				.def("__pretty__", &DynamicCallableInstance::ToPrettyString)
				;
		}
		
	private:
		boost::python::object _callable;
		std::vector< gcroot<System::Type^> > _ptypes;
	};

	template<class InvokerType> struct DynamicOverloadResolver : private DynamicObjectDetail, InvocationForwarding
	{
		boost::python::object Invoke(const boost::python::tuple &args)
		{
			int i = FindSuitableOverloadIndex(args);

			if (i == -1)
			{
				throw_exception("No suitable overload");
				throw std::runtime_error("No suitable overload");
			}

			PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG(
				boost::python::extract<std::string>(boost::python::str("Calling: ") + _invokers[i].GetSignature()), args);

			return _invokers[i].Invoke(args);
		}

		System::Type ^ GetDeclaringType() const
		{
			if (_invokers.empty())
				return nullptr;

			return _invokers[0].GetDeclaringType();
		}

		std::string GetCallableType() const
		{
			if (_invokers.empty())
				return std::string();

			return _invokers[0].GetCallableType();
		}

		std::string GetName() const
		{
			if (_invokers.empty())
				return std::string();

			return _invokers[0].GetName();
		}
		
		boost::python::list GetOverloadedSignatures() const
		{
			boost::python::list lst;

			const int nInvokers = _invokers.size();
			for (int i = 0; i != nInvokers; ++i)
			{
				lst.append(_invokers[i].GetSignature());
			}

			return lst;
		}
		
		boost::python::str ToString() const
		{
			if (_invokers.empty())
				return boost::python::str();

			return _invokers[0].ToString();
		}

		boost::python::str ToReprString() const
		{
			if (_invokers.empty())
				return boost::python::str();

			return _invokers[0].ToReprString();
		}

		boost::python::str ToPrettyString() const
		{
			return boost::python::str("\n").join(GetOverloadedSignatures());
		}

		boost::python::list GetOverloads() const
		{
			boost::python::list rv;
			const int nInvokers = _invokers.size();
			for (int i = 0; i != nInvokers; ++i)
			{
				auto ci = _invokers[i].GetCallable();
				rv.append(ci);
			}
			return rv;
		}

        boost::python::object GetInstance() const
        {
			if (_invokers.empty())
				return boost::python::object();

            return _invokers[0].GetInstance();
        }


		int GetNumOverloads() const 
		{
			return _invokers.size();
		}

		void Add(InvokerType invoker)
		{
			_invokers.push_back(invoker);
		}

		int FindSuitableOverloadIndex(const boost::python::tuple &args) const;

		InvokerType GetSpecificOverload1(const boost::python::object &args) const;
		InvokerType GetSpecificOverload(const boost::python::tuple &args) const;

		static void Register(const std::string &name)
		{
			using namespace boost::python;

			PYDOTNET_REGISTER_PRINT_DEBUG(name);

			class_<DynamicOverloadResolver, bases<InvocationForwarding>>(name.c_str())
				.add_property("Name", &DynamicOverloadResolver::GetName, "Name")
				.add_property("__func__", &DynamicOverloadResolver::GetOverloads)
				.add_property("__instance__", &DynamicOverloadResolver::GetInstance)
				.def("AddOverload", &DynamicOverloadResolver::Add, "Adds method overload")
				.def("__getitem__", &DynamicOverloadResolver::GetSpecificOverload1, "Gets managed method overload that matches signature")
				.def("__len__", &DynamicOverloadResolver::GetNumOverloads)
				.def("__str__", &DynamicOverloadResolver::ToString)
				.def("__repr__", &DynamicOverloadResolver::ToReprString)
				.def("__pretty__", &DynamicOverloadResolver::ToPrettyString);
				;
		}

	private:
		std::vector<InvokerType> _invokers;
	};

	struct DynamicMethodInvoker : DynamicCallable
	{
		DynamicMethodInvoker(MethodInfo ^mi, System::Object ^obj) : _mi(mi), _obj(obj), DynamicCallable(mi->ReturnType, mi->Name)
		{
		}

		// Member method
		DynamicMethodInvoker(const ObjectHandle &mi, const ObjectHandle &obj) : _mi(safe_cast<MethodInfo^>(mi.GetObject())), _obj(obj.GetObject()), 
			DynamicCallable(
			safe_cast<MethodInfo^>(mi.GetObject())->ReturnType, 
			safe_cast<MethodInfo^>(mi.GetObject())->Name)
		{
		}

		// Static method
		DynamicMethodInvoker(const ObjectHandle &mi) : _mi(safe_cast<MethodInfo^>(mi.GetObject())), 
			DynamicCallable(
			safe_cast<MethodInfo^>(mi.GetObject())->ReturnType, 
			safe_cast<MethodInfo^>(mi.GetObject())->Name)
		{
		}

		array<ParameterInfo ^> ^GetParameters() const
		{
			return _mi->GetParameters();
		}

		System::Object ^DoInvoke(array<System::Object ^> ^args)
		{
			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Calling method...");
			{
				ReleaseGIL lk;
				return _mi->Invoke(_obj, args);
			}
		}

		System::Delegate ^GetDelegate() const;

		System::Type ^ GetDeclaringType() const
		{
			return _mi->DeclaringType;
		}

		System::Object ^ GetCallableInfo() const
		{
			return _mi;
		}
		
		std::string GetCallableType() const
		{
			return std::string(IsStatic() ? "static " : (IsUnboundMethod() ? "unbound " : "bound ")) + "method";
		}

		bool IsStatic() const
		{
			return _mi->IsStatic;
		}

		bool IsUnboundMethod() const
		{
			return (_obj.operator System::Object ^() == nullptr);
		}

        boost::python::object GetInstance() const
        {
            if (_obj.operator System::Object ^() == nullptr)
            {   
                return boost::python::object();
            }

            return boost::python::object(DynamicObjectHandle(_obj));
        }

		boost::python::object Clone() const
		{
			DynamicMethodInvoker self(_mi, _obj);
			return boost::python::object(self);
		}

		static void Register(const std::string &name, const std::string &overloadsName)
		{
			using namespace boost::python;

			PYDOTNET_REGISTER_PRINT_DEBUG(name);

			class_<DynamicMethodInvoker, bases<DynamicCallable> >(name.c_str(), init<const ObjectHandle &>())
				.def(init<const ObjectHandle &, const ObjectHandle &>())
				.def("__getitem__", &DynamicCallable::GetDynamicCallableInstance1)
				;

			RegisterOverloads(overloadsName);
		}


	private:
		gcroot<MethodInfo ^> _mi;
		gcroot<System::Object ^> _obj;

		static void RegisterOverloads(const std::string &name);
	};

	struct DynamicConstructorInvoker : DynamicCallable
	{
		DynamicConstructorInvoker(ConstructorInfo ^mi) : _mi(mi), DynamicCallable(mi->DeclaringType, "__init__")
		{}

		DynamicConstructorInvoker(const ObjectHandle &mi)
			: _mi(safe_cast<ConstructorInfo ^>(mi.GetObject())),
			DynamicCallable(
			safe_cast<MethodInfo^>(mi.GetObject())->DeclaringType, 
			"__init__")
		{}

		array<ParameterInfo ^> ^GetParameters() const
		{
			return _mi->GetParameters();
		}

		System::Object ^DoInvoke(array<System::Object ^> ^args)
		{
			ReleaseGIL lk;
			return _mi->Invoke(args);
		}
		
		System::Object ^ GetCallableInfo() const
		{
			return _mi;
		}
		
		System::Type ^ GetDeclaringType() const
		{
			return _mi->DeclaringType;
		}

		std::string GetCallableType() const
		{
			return "constructor";
		}

		bool IsStatic() const
		{
			return true;
		}

        boost::python::object GetInstance() const
        {
            return boost::python::object();
        }

		boost::python::object Clone() const
		{
			DynamicConstructorInvoker self(_mi);
			return boost::python::object(self);
		}

		static void Register(const std::string &name, const std::string &overloadsName)
		{
			using namespace boost::python;

			PYDOTNET_REGISTER_PRINT_DEBUG(name);

			class_<DynamicConstructorInvoker, bases<DynamicCallable> >(name.c_str(), init<const ObjectHandle &>())
				.def("__getitem__", &DynamicCallable::GetDynamicCallableInstance1)
				;

			RegisterOverloads(overloadsName);
		}

	private:
		gcroot<ConstructorInfo ^> _mi;

		static void RegisterOverloads(const std::string &name);
	};

	struct DynamicTypeHandle : DynamicObjectHandle, InvocationForwarding
	{
		DynamicTypeHandle(System::Type ^typ) : DynamicObjectHandle(nullptr, typ)
		{
			Init();
		}

		explicit DynamicTypeHandle(const ObjectHandle &handle) : DynamicObjectHandle(nullptr, dynamic_cast<System::Type^>(handle.GetObject()))
		{
			Init();

			if (GetTypeObject() == nullptr)
			{
				throw_invalid_cast("Type expected");
				throw std::runtime_error("Type expected");
			}
		}

		boost::python::object GetTypeId() const 
		{
			return _typeid;
		}

		boost::python::object GetConstructor() const
		{
			return _constructor;
		}

		DynamicTypeHandle GetSpecializedType(const boost::python::tuple &args)
		{
			PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("GetSpecializedType", args);

			System::Type ^typ = GetTypeObject();

			int n = boost::python::len(args);
			array<System::Type ^> ^typeArgs = gcnew array<System::Type ^>(n);

			for (int i = 0; i != n; ++i)
			{
				boost::python::object arg = args[i];

				boost::python::extract<const DynamicTypeHandle &> maybeTypeHandle(arg);
				if (!maybeTypeHandle.check())
				{
					arg = arg.attr("__class__");
					maybeTypeHandle = boost::python::extract<const DynamicTypeHandle &>(arg);
				}
				if (maybeTypeHandle.check())
				{
					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Type class");
					const DynamicTypeHandle &typeHandle = maybeTypeHandle;
					typeArgs[i] = typeHandle.GetTypeObject();
					continue;
				}

				boost::python::extract<const DynamicObjectHandle &> maybeObjectHandle(arg);
				if (maybeObjectHandle.check())
				{
					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Type instance");
					const DynamicObjectHandle &obj = maybeObjectHandle;
					typeArgs[i] = obj.GetTypeObject();
					continue;
				}

				throw_invalid_cast("Type expected");
				throw std::runtime_error("Type expected");
			}

			try
			{
				System::Type ^genTyp = typ->MakeGenericType(typeArgs);
				return genTyp;
			}
			PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
		}

		boost::python::object Invoke(const boost::python::tuple &args)
		{
			PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("Calling constructor", args);
			if (_constructor.is_none())
			{
				throw_exception("No constructor available");
				throw std::runtime_error("No constructor available");
			}

			if (GetTypeObject()->IsGenericTypeDefinition)
			{
				throw_exception("Unspecialized generic type");
				throw std::runtime_error("Unspecialized generic type");
			}


			boost::python::extract<InvocationForwarding &> maybeInvoker(_constructor);
			if (maybeInvoker.check())
			{
				InvocationForwarding &invoker = maybeInvoker;
				return invoker.Invoke(args);
			}

			if (boost::python::len(args) == 0)
			{
				return _constructor();
			}

			throw_exception("Constructor not available");
			throw std::runtime_error("Constructor not available");
		}

		boost::python::object GetItem(const boost::python::object &arg)
		{
			if (GetTypeObject()->IsGenericTypeDefinition)
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("Specializing generic type", arg);
				boost::python::extract<boost::python::tuple> maybeTuple(arg);
				if (maybeTuple.check())
				{
					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Specializing generic type with multiple parameters...");
					return boost::python::object(GetSpecializedType(maybeTuple));
				}
				else
				{
					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Specializing generic type with single parameter...");
					boost::python::list lst;
					lst.append(arg);
					return boost::python::object(GetSpecializedType(boost::python::tuple(lst)));
				}
			}
			else
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("Obtaining constructor", arg);
				if (_constructor.is_none())
				{
					throw_exception("No constructor available");
					throw std::runtime_error("No constructor available");
				}

				boost::python::extract<DynamicOverloadResolver<DynamicConstructorInvoker> &> maybeResolver(_constructor);
				if (maybeResolver.check())
				{
					DynamicOverloadResolver<DynamicConstructorInvoker> &resolver = maybeResolver;
					return boost::python::object(resolver.GetSpecificOverload1(arg));
				}

				throw_exception("Constructor not available");
				throw std::runtime_error("Constructor not available");
			}
		}

		bool operator == (const DynamicObjectHandle &b) const
		{
			return GetTypeObject()->Equals(b.GetTypeObject());
		}

		bool operator != (const DynamicObjectHandle &b) const
		{
			return !((*this) == b);
		}

		static void Register(const std::string &name)
		{
			using namespace boost::python;

			PYDOTNET_REGISTER_PRINT_DEBUG(name);

			class_<DynamicTypeHandle, bases<DynamicObjectHandle, InvocationForwarding> >(name.c_str(), init<const ObjectHandle &>())
				.add_property("__typeid__", &DynamicTypeHandle::GetTypeId, "Allows access to managed type information")
				.add_property("__createinstance__", &DynamicTypeHandle::GetConstructor, "Gets constructor or constructor overloads")
				.def("__getitem__", &DynamicTypeHandle::GetItem, 
				"Gets either specialized generic type if it is generic unspecialized type or constructor overload that matches signature")
				.def(self == self)
				.def(self != self)
				;
		}

	private:
		boost::python::object _typeid;
		boost::python::object _constructor;

		struct CreateValueTypeInstance
		{
			CreateValueTypeInstance(System::Type ^typ): _typ(typ)
			{}

			DynamicObjectHandle Invoke()
			{
				return System::Activator::CreateInstance(_typ);
			}

			static boost::python::object MakeConstructor(System::Type ^typ)
			{
				using namespace boost::python;

				CreateValueTypeInstance ctor(typ);

				auto f = boost::bind(&CreateValueTypeInstance::Invoke, ctor);
				auto g = make_function(
					f,
					default_call_policies(),
					boost::mpl::vector1<DynamicObjectHandle>());

				return g;
			}

		private:
			gcroot<System::Type ^> _typ;
		};

		void Init()
		{
			System::Type ^typ = GetTypeObject();

			// Type id
			_typeid = boost::python::object(DynamicObjectHandle(typ));

			// Instance constructors
			auto ctors = typ->GetConstructors();
			const int n = ctors->Length;

			if (n == 0)
			{
				if (typ->IsValueType)
				{
					_constructor = CreateValueTypeInstance::MakeConstructor(typ);
				}
				else
				{
					return;
				}
			}
			else if (n == 1)
			{
				DynamicConstructorInvoker method(ctors[0]);
				_constructor = boost::python::object(method);
			}
			else
			{
				DynamicOverloadResolver<DynamicConstructorInvoker> overloads;

				for (int i = 0; i != n; ++i)
				{
					overloads.Add(DynamicConstructorInvoker(ctors[i]));
				}
				_constructor = boost::python::object(overloads);
			}
		}
	};

	struct DynamicTypesCache
	{
		void Refresh1(System::Reflection::Assembly ^assembly);

		void Refresh()
		{
			PYDOTNET_REGISTER_PRINT_DEBUG("Refreshing type cache...");

			auto assemblies = System::AppDomain::CurrentDomain->GetAssemblies();

			for (int i = 0; i != assemblies->Length; ++i)
			{
				Refresh1(assemblies[i]);
			}
		}

		static DynamicTypesCache &GetInstance()
		{
			if (sInstance == nullptr)
			{
				throw_null_reference();
				throw std::runtime_error("null");
			}
			return *sInstance;
		}

		static boost::python::object GetInstanceWrapper()
		{
			boost::python::pointer_wrapper<DynamicTypesCache*> ptr(sInstance);
			return boost::python::object(ptr);
		}


		boost::python::list GetCachedTypes() const
		{
			boost::python::list r;
			for (auto it = Types.cbegin(), end = Types.cend(); it != end; ++it)
			{
				r.append((*it).second);
			}
			return r;
		}
		
		boost::python::list GetCachedNamespaces() const
		{
			boost::python::list r;
			for (auto it = Namespaces.cbegin(), end = Namespaces.cend(); it != end; ++it)
			{
				r.append(*it);
			}
			return r;
		}
		
		boost::python::list GetCachedAssemblies() const
		{
			boost::python::list r;
			for (auto it = Assemblies.cbegin(), end = Assemblies.cend(); it != end; ++it)
			{
				r.append(*it);
			}
			return r;
		}

		static void Register(const std::string &name)
		{
			using namespace boost::python;

			PYDOTNET_REGISTER_PRINT_DEBUG(name);

			class_<DynamicTypesCache>(name.c_str(), no_init)
				.add_property("Types", &DynamicTypesCache::GetCachedTypes, "Cached types")
				.add_property("Namespaces", &DynamicTypesCache::GetCachedNamespaces, "Cached namespaces")
				.add_property("Assemblies", &DynamicTypesCache::GetCachedAssemblies, "Cached assemblies")
				.def("Refresh", &DynamicTypesCache::Refresh, "Refresh cached assemblies");

			sInstance = new DynamicTypesCache;
		}

		std::map<std::string, DynamicTypeHandle> Types;
		std::set<std::string> Namespaces;
		std::set<std::string> Assemblies;

	private:
		static DynamicTypesCache *sInstance;
	};

	struct DynamicNamespace : private DynamicObjectDetail
	{
		DynamicNamespace()
		{}

		DynamicNamespace(const std::string &name) : _name(name)
		{}

		std::string GetName() const
		{
			return _name;
		}

		std::string GetQualifiedName(const std::string &name) const
		{
			return (_name.empty() ? name : _name + "." + name);
		}

		boost::python::object GetMember(const std::string &name) const;

		boost::python::list GetMembers() const;

		void ImportTypesInto(boost::python::object &targetNamespace, const boost::python::object &names) const;

		static void Register(const std::string &name)
		{
			using namespace boost::python;

			PYDOTNET_REGISTER_PRINT_DEBUG(name);

			class_<DynamicNamespace>(name.c_str())
				.def(init<const std::string &>())
				.def("__getitem__", &DynamicNamespace::GetMember, "Gets member of the namespace")
				.def("__getattr__", &DynamicNamespace::GetMember, "Gets member of the namespace")
				.def("__dir__", &DynamicNamespace::GetMembers, "Gets names of all members of the namespace")
				.def("__str__", &DynamicNamespace::GetName, "Gets qualified name of the namespace")
				;
		}

	private:
		std::string _name;
	};

	struct DynamicAppDomain : DynamicNamespace
	{
		void LoadAssembly(const std::string &name)
		{
			try
			{
				if (boost::algorithm::iends_with(name, ".dll"))
				{
					System::Reflection::Assembly::LoadFrom(ConvertToManagedString(name));
				}
				else
				{
					System::Reflection::Assembly::Load(ConvertToManagedString(name));
				}
				DynamicTypesCache::GetInstance().Refresh();
			}
			PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
		}

		void LoadSource(const char *source, const std::string &outputFile, boost::python::object assemblies, const std::string &compilerOptions);

		boost::python::list GetLoadedAssemblies()
		{
			boost::python::list lst;

			auto assemblies = System::AppDomain::CurrentDomain->GetAssemblies();
			int n = assemblies->Length;

			for (int i = 0; i != n; ++i)
			{
				DynamicObjectHandle x(assemblies[i]);
				lst.append(x);
			}

			return lst;
		}

		boost::python::list GetTypes() const
		{
			boost::python::list lst;

			for (auto iter = DynamicTypesCache::GetInstance().Types.cbegin();
				iter != DynamicTypesCache::GetInstance().Types.cend();
				++iter)
			{
				lst.append(iter->first);
			}

			return lst;
		}

		boost::python::list GetNamespaces() const
		{
			boost::python::list lst;

			for (auto iter = DynamicTypesCache::GetInstance().Namespaces.cbegin();
				iter != DynamicTypesCache::GetInstance().Namespaces.cend();
				++iter)
			{
				lst.append(*iter);
			}

			return lst;
		}

		void ResolveUsing1(const std::string &name)
		{
			boost::python::object locals(boost::python::borrowed(PyEval_GetLocals()));

			DynamicNamespace(name).ImportTypesInto(locals, boost::python::object());
		}

		void ResolveUsing2(const std::string &name, const boost::python::list &names)
		{
			boost::python::object locals(boost::python::borrowed(PyEval_GetLocals()));

			DynamicNamespace(name).ImportTypesInto(locals, names);

			const int n = boost::python::len(names);

			for (int i = 0; i != n; ++i)
			{
				std::string name = boost::python::extract<std::string>(names[i]);
				if (!locals.contains(name))
				{
					throw_exception(name + " type not found");
					throw std::runtime_error(name + "type not found");
				}
			}
		}

		static void Register(const std::string &name)
		{
			using namespace boost::python;

			PYDOTNET_REGISTER_PRINT_DEBUG(name);

			class_<DynamicAppDomain, bases<DynamicNamespace> >(name.c_str(), no_init)
				.add_property("LoadedAssemblies", &DynamicAppDomain::GetLoadedAssemblies, "Gets list of loaded assemblies")
				.add_property("Types", &DynamicAppDomain::GetTypes, "Gets qualified names of all loaded types")
				.add_property("Namespaces", &DynamicAppDomain::GetNamespaces, "Gets qualified names of all namespaces loaded")
				.def("LoadAssembly", &DynamicAppDomain::LoadAssembly, "Loads assembly")
				.def("LoadSource", &DynamicAppDomain::LoadSource, "Compiles C# source code and loads resultant assembly into memory")
				.def("ResolveUsing", &DynamicAppDomain::ResolveUsing1, "Imports into global python namespace all types from given namespace."
				" An equivalent of 'from M import *'")
				.def("ResolveUsing", &DynamicAppDomain::ResolveUsing2, "Imports into global python namespace only selected types from given namespace"
				" An equivalent of 'from M import A,B,C'")
				;

			DynamicTypesCache::GetInstance().Refresh();
		}
	};


	//////
	//
	// Global Methods
	//
	//

	inline bool IsInstance(const DynamicObjectHandle &a, const DynamicTypeHandle &b)
	{
		return b.GetTypeObject()->IsAssignableFrom(a.GetTypeObject());
	}

	inline boost::python::object GetType(boost::python::object obj)
	{
		return obj.attr("__typeid__");
	}

	inline boost::python::object GetClass(boost::python::object obj)
	{
		return obj.attr("__classid__");
	}

	inline boost::python::object GetConstructor(boost::python::object obj)
	{
		return obj.attr("__createinstance__");
	}

} // namespace InteropPython

#endif // INCLUDED...
