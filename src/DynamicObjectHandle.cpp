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
		
	boost::python::object DynamicObjectHandle::_getAttrHook;
	boost::python::object DynamicObjectHandle::_getAttrBase;

	void DynamicObjectHandle::Init()
	{
		if (GetObject() != nullptr)
		{
			if (_typ.operator System::Type ^() == nullptr)
			{
				_typ = GetObject()->GetType();
			}
			_flags = (int)BindingFlags::Instance;
		}
		else
		{
			_flags = (int)BindingFlags::Static;
		}
	}

	boost::python::object DynamicObjectHandle::GetProperty(const std::string &name)
	{
		PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("GetProperty", name);

		if (GetTypeObject() == nullptr)
		{
			throw_invalid_attribute();
			throw std::runtime_error("Invalid attribute");
		}

		auto pp = _cache.find(name);
		if (pp != _cache.cend())
		{
			return (pp->second);
		}

		msclr::interop::marshal_context context;

		System::Object ^obj = GetObject();

		array<MemberInfo ^> ^pi = _typ->GetMember(ConvertToManagedString(name), BindingFlags::FlattenHierarchy 
			| BindingFlags::Public
			| BindingFlags::Instance 
			| BindingFlags::Static);

		if (pi == nullptr || pi->Length < 1)
		{
			throw_invalid_attribute();
			throw std::runtime_error("Invalid attribute");
		}

		try 
		{
			boost::python::object p = DoGetProperty(obj, pi);

			if (IsConstantProperty(pi[0]))
			{
				_cache.insert(std::make_pair(name, p));
			}
			return p;
		}
		PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
	}

	void DynamicObjectHandle::SetProperty(const char *name, boost::python::object val)
	{
		PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("SetProperty");

		if (GetTypeObject() == nullptr)
		{
			throw_invalid_attribute();
			throw std::runtime_error("Invalid attribute");
		}

		System::Object ^obj = GetObject();
		BindingFlags flags = (BindingFlags)_flags;

		array<MemberInfo ^> ^pi = _typ->GetMember(
			gcnew System::String(name), flags | BindingFlags::Public | BindingFlags::FlattenHierarchy );

		if (pi == nullptr || pi->Length < 1)
		{
			throw_invalid_attribute();
		}

		try
		{
			if (!DoSetProperty(obj, pi[0], val))
			{
				throw_invalid_cast();
				throw std::runtime_error("Invalid cast");
			}
		}
		PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
	}

	boost::python::list DynamicObjectHandle::GetProperties()
	{
		if (GetTypeObject() == nullptr)
		{
			return boost::python::list();
		}

		boost::python::list names;

		array<MemberInfo ^> ^properties = _typ->GetMembers(BindingFlags::FlattenHierarchy 
			| BindingFlags::Public
			| BindingFlags::Instance 
			| BindingFlags::Static);

		msclr::interop::marshal_context context;

		for (int i = 0; i != properties->Length; ++i)
		{
			MemberInfo ^pi = properties[i];

			std::string name = context.marshal_as<std::string>(pi->Name);

			names.append(name);
		}

		return names;
	}

	boost::python::object DynamicObjectHandle::GetItem(boost::python::object key)
	{
		if (GetTypeObject() == nullptr)
		{
			throw_invalid_attribute();
			throw std::runtime_error("Invalid attribute");
		}

		try
		{
			if (_typ->IsArray)
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("GetItem from array...");
				int index = boost::python::extract<int>(key);
				System::Array ^ arr = safe_cast<System::Array ^>(GetObject());
				System::Object ^val = arr->GetValue(index);
				return ConvertToPython(val);
			}
			else
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("GetItem from list or dict...");
				auto mi = _typ->GetMethod("get_Item");
				auto mipi = mi->GetParameters()[0];
				auto args = gcnew array<System::Object ^>(1);
				args[0] = ConvertToManaged(key, mipi->ParameterType);
				System::Object ^val;
				{
					ReleaseGIL lk;
					val = mi->Invoke(GetObject(), args);
				}
				return ConvertToPython(val);
			}
		}
		PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
	}

	void DynamicObjectHandle::SetItem(boost::python::object key, boost::python::object val)
	{
		if (GetTypeObject() == nullptr)
		{
			throw_invalid_attribute();
			throw std::runtime_error("Invalid attribute");
		}

		try
		{
			if (_typ->IsArray)
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("SetItem in array...");
				int index = boost::python::extract<int>(key);
				System::Array ^arr = safe_cast<System::Array ^>(GetObject());
				System::Object ^value = ConvertToManaged(val, _typ->GetElementType());
				arr->SetValue(value, index);
			}
			else if (_typ->IsGenericType)
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("SetItem in list or dict...");
				auto mi = _typ->GetMethod("set_Item");
				auto mipi1 = mi->GetParameters()[0];
				auto mipi2 = mi->GetParameters()[1];
				auto args = gcnew array<System::Object ^>(2);
				args[0] = ConvertToManaged(key, mipi1->ParameterType);
				args[1] = ConvertToManaged(val, mipi2->ParameterType);
				{
					ReleaseGIL lk;
					mi->Invoke(GetObject(), args);
				}
			}
			else
			{
				throw_exception("Unsupported");
				throw std::runtime_error("Unsupported");
			}
		}
		PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
	}

	int DynamicObjectHandle::GetLength()
	{
		if (GetTypeObject() == nullptr)
		{
			throw_invalid_attribute();
			throw std::runtime_error("Invalid attribute");
		}

		try
		{
			if (_typ->IsArray)
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("GetLength of array...");
				System::Array ^arr = safe_cast<System::Array ^>(GetObject());
				return arr->GetLength(0);
			}
			else
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("GetLength of list...");
				auto pi = _typ->GetProperty("Count");
				System::Object ^val;
				{
					ReleaseGIL lk;
					val = pi->GetValue(GetObject(), gcnew array<System::Int32 ^>(0));
				}
				return System::Convert::ToInt32(val);
			}
		}
		PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
	}

	boost::python::object DynamicObjectHandle::GetClassId() const
	{
		if (GetTypeObject() == nullptr)
		{
			throw_invalid_attribute();
			throw std::runtime_error("Invalid attribute");
		}

		std::string name = ConvertToUnmanaged(_typ->Name);
		std::string qname;
		if (_typ->Namespace == nullptr)
		{
			std::string ns = ConvertToUnmanaged(_typ->Namespace);
			qname = ns + "." + name;
		}
		else 
		{
			qname = name;
		}

		auto it = DynamicTypesCache::GetInstance().Types.find(qname);
		if (it != DynamicTypesCache::GetInstance().Types.cend())
		{
			return boost::python::object(it->second);
		}

		return boost::python::object(DynamicTypeHandle(_typ));
	}

	boost::python::object DynamicObjectHandle::GetIter()
	{
		try
		{
			auto iterable = safe_cast<System::Collections::IEnumerable ^>(GetObject());
			return boost::python::object(DynamicIterator(iterable->GetEnumerator()));
		}
		PYDOTNET_HANDLE_MANAGED_EXCEPTION(err);
	}

	boost::python::str DynamicObjectHandle::ToReprString()
	{
		if (GetTypeObject() == nullptr)
		{
			return boost::python::str("None");
		}

		// Used when calling: repr(x), or expression result in interactive python shell
		System::Object ^obj = GetObject();

		if (obj == nullptr)
		{
			return
				boost::python::str(
				boost::python::str("<class ") +
				boost::python::str(ConvertToUnmanaged(_typ->Name)) +
				boost::python::str(">"));
		}

		if (_typ->IsPrimitive)
		{
			return ToString();
		}

		if (System::String::typeid->Equals(_typ))
		{
			return boost::python::str(boost::python::str("'") + ToString() + boost::python::str("'"));
		}

		if (_typ->IsArray)
		{
			boost::python::object r = boost::python::str("[");
			boost::python::str curr;
			boost::python::str next(", ");

			auto arr = safe_cast<System::Array ^>(GetObject());
			for (int i = 0; i != arr->GetLength(0); ++i)
			{
				DynamicObjectHandle item(arr->GetValue(i));

				r = r + curr + item.ToReprString();
				curr = next;
			}

			return boost::python::str(r + boost::python::str("]"));
		}

		if (_typ->IsGenericType)
		{
			if (_typ->GetGenericTypeDefinition()->Equals(System::Collections::Generic::KeyValuePair<
				System::Object^, System::Object ^>::typeid->GetGenericTypeDefinition()))
			{
				boost::python::object key = GetProperty("Key");
				boost::python::object val = GetProperty("Value");

				return boost::python::str(
					boost::python::str("{") +
					InteropPython::Repr(key) + boost::python::str(": ") + 
					InteropPython::Repr(val) +
					boost::python::str("}"));
			}

			if (_typ->GetGenericTypeDefinition()->Equals(System::Tuple<
				System::Object^>::typeid->GetGenericTypeDefinition()))
			{
				boost::python::object item1 = GetProperty("Item1");

				return boost::python::str(
					boost::python::str("(") + InteropPython::Repr(item1) + boost::python::str(",)"));
			}

			if (_typ->GetGenericTypeDefinition()->Equals(System::Tuple<
				System::Object^, System::Object^>::typeid->GetGenericTypeDefinition()))
			{
				boost::python::object item1 = GetProperty("Item1");
				boost::python::object item2 = GetProperty("Item2");

				return boost::python::str(
					boost::python::str("(") +
					InteropPython::Repr(item1) + boost::python::str(", ") +
					InteropPython::Repr(item2) +
					boost::python::str(")"));
			}

			if (_typ->GetGenericTypeDefinition()->Equals(System::Tuple<
				System::Object^, System::Object^, System::Object^>::typeid->GetGenericTypeDefinition()))
			{
				boost::python::object item1 = GetProperty("Item1");
				boost::python::object item2 = GetProperty("Item2");
				boost::python::object item3 = GetProperty("Item3");

				return boost::python::str(
					boost::python::str("(") +
					InteropPython::Repr(item1) + boost::python::str(", ") +
					InteropPython::Repr(item2) + boost::python::str(", ") +
					InteropPython::Repr(item3) +
					boost::python::str(")"));
			}

			if (_typ->GetGenericTypeDefinition()->Equals(System::Tuple<
				System::Object^, System::Object^, System::Object^, System::Object^>::typeid->GetGenericTypeDefinition()))
			{
				boost::python::object item1 = GetProperty("Item1");
				boost::python::object item2 = GetProperty("Item2");
				boost::python::object item3 = GetProperty("Item3");
				boost::python::object item4 = GetProperty("Item4");

				return boost::python::str(
					boost::python::str("(") +
					InteropPython::Repr(item1) + boost::python::str(", ") +
					InteropPython::Repr(item2) + boost::python::str(", ") +
					InteropPython::Repr(item3) + boost::python::str(", ") +
					InteropPython::Repr(item4) +
					boost::python::str(")"));
			}

			if (System::Collections::IList::typeid->IsAssignableFrom(_typ))
			{
				boost::python::object r = boost::python::str("[");
				boost::python::str curr;
				boost::python::str next(", ");

				auto list = safe_cast<System::Collections::IList ^>(obj);
				for (int i = 0; i != list->Count; ++i)
				{
					DynamicObjectHandle item(list[i]);

					r = r + curr + item.ToReprString();
					curr = next;
				}

				return boost::python::str(r + boost::python::str("]"));
			}

			if (System::Collections::IDictionary::typeid->IsAssignableFrom(_typ))
			{
				boost::python::object r = boost::python::str("{");
				boost::python::str curr;
				boost::python::str next(", ");

				auto dict = safe_cast<System::Collections::IDictionary ^>(obj);
				auto keys = dict->Keys->GetEnumerator();

				while (keys->MoveNext())
				{
					DynamicObjectHandle key(keys->Current);
					DynamicObjectHandle val(dict[keys->Current]);

					r = r + curr + key.ToReprString() + boost::python::str(": ") + val.ToReprString();
					curr = next;
				}

				return boost::python::str(r + boost::python::str("}"));
			}
		}

		if (_typ->IsEnum)
		{
			return 
				boost::python::str(
				boost::python::str("<enum ") +
				boost::python::str(ConvertToUnmanaged(_typ->Name)) +
				boost::python::str(".") +
				boost::python::str(ConvertToUnmanaged(obj->ToString())) +
				boost::python::str(">"));
		}

		if (System::Type::typeid->IsInstanceOfType(obj))
		{
			return
				boost::python::str(
				boost::python::str("<") +
				boost::python::str(ConvertToUnmanaged(safe_cast<System::Type ^>(obj)->Name)) +
				boost::python::str(" type 'instance'>"));
		}

		return
			boost::python::str(
			boost::python::str("<") +
			boost::python::str(ConvertToUnmanaged(_typ->Name)) +
			boost::python::str(" instance") +
			boost::python::str(">"));
	}

	boost::python::str DynamicObjectHandle::ToPrettyString()
	{
		if (GetTypeObject() == nullptr)
		{
			throw_invalid_attribute();
			throw std::runtime_error("Invalid attribute");
		}

		boost::python::list pairs;

		System::Object ^obj = GetObject();

		if (_typ->IsEnum && obj != nullptr)
		{
			return 
				boost::python::str(
				boost::python::str(ConvertToUnmanaged(obj->ToString())) +
				boost::python::str(" value of ") +
				boost::python::str(ConvertToUnmanaged(_typ->Name)));
		}

		if (obj == nullptr) 
		{
			obj = _typ;
		}

		BindingFlags flags = (BindingFlags)_flags;
		array<MemberInfo ^> ^properties = _typ->GetMembers(flags | BindingFlags::Public | BindingFlags::FlattenHierarchy);

		msclr::interop::marshal_context context;

		for (int i = 0; i != properties->Length; ++i)
		{
			MemberInfo ^pi = properties[i];

			System::Type ^pt = GetPropertyType(pi);
			if (pt == nullptr)
			{
				// Skip everything that is neither Property nor Field
				continue;
			}

			std::string name = context.marshal_as<std::string>(pi->Name);

			boost::python::list pair;
			pair.append(boost::python::str(name));

			if (!pt->IsPrimitive && !pt->Equals(System::String::typeid))
			{
				std::string propTypeName = context.marshal_as<std::string>(pt->Name);
				pair.append(
					boost::python::str(
					boost::python::str("instance of ") +
					boost::python::str(propTypeName)));
			}
			else
			{
				try
				{
					boost::python::object val = DoGetProperty(obj, pi);
					pair.append(boost::python::str(val));
				}
				catch(System::Exception ^) {}
			}

			pairs.append(boost::python::str(":\t").join(pair));
		}

		return 
			boost::python::str(
			boost::python::str(GetTypeName()) +
			boost::python::str(":\n\t") +
			boost::python::str("\n\t").join(pairs));
	}

	boost::python::object DynamicObjectHandle::GetAttr(const std::string &name)
	{
		if (_getAttrHook.is_none())
		{
			return GetProperty(name);
		}
		else
		{
			// This allows one to wrap properties with some custom python code
			// We pass original GetProperty() to python code, which may or may not call it.

			return _getAttrHook(this, name, _getAttrBase);
		}
	}

	void DynamicObjectHandle::SetGetAttrHook(boost::python::object hook)
	{
		_getAttrHook = hook;
		_getAttrBase = boost::python::make_function(&DynamicObjectHandle::GetProperty);
	}



}// namespace InteropPython
