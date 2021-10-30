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

	DynamicCallableInstance::DynamicCallableInstance(boost::python::object callable, boost::python::tuple args) 
		: _callable(callable)
	{
		const int nArgs = boost::python::len(args);
				
		for (int i = 0; i != nArgs; ++i)
		{
			boost::python::object item = args[i];
			boost::python::extract<DynamicTypeHandle &> maybeType(item);
			if (!maybeType.check())
			{
				item = item.attr("__class__");
				maybeType = boost::python::extract<DynamicTypeHandle &>(item);
			}
			if (maybeType.check())
			{
				const DynamicTypeHandle &typeArg = maybeType;
				_ptypes.push_back(typeArg.GetTypeObject());
			}
			else
			{
				throw_exception("Type expected");
				throw std::runtime_error("Type expected");
			}
		}
	}
		
	boost::python::list DynamicCallableInstance::GetParameterTypes() const
	{
		boost::python::list lst;

		const int nArgs = _ptypes.size();

		for (int i = 0; i != nArgs; ++i)
		{
			DynamicObjectHandle ptype(_ptypes[i], nullptr);
			lst.append(boost::python::object(ptype));
		}

		return lst;
	}

	boost::python::object DynamicCallable::GetDynamicCallableInstance1(const boost::python::object &arg) const
	{
		PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("Looking up overload", arg);
		if (arg.is_none())
		{
			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Looking up overload with no parameters...");
			return GetDynamicCallableInstance(boost::python::tuple());
		}
		else
		{
			boost::python::extract<boost::python::tuple> maybeTuple(arg);
			if (maybeTuple.check())
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Looking up overload with multiple parameters...");
				return GetDynamicCallableInstance(maybeTuple);
			}
			else
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Looking up overload with single parameter...");
				boost::python::list lst;
				lst.append(arg);
				return GetDynamicCallableInstance(boost::python::tuple(lst));
			}
		}
	}
		
	boost::python::object DynamicCallable::GetDynamicCallableInstance(const boost::python::tuple &args) const
	{
		boost::python::object self = Clone();
		DynamicCallableInstance dci(self, args);
		return boost::python::object(dci);
	}

	template<typename InvokerType>
	InvokerType DynamicOverloadResolver<InvokerType>::GetSpecificOverload1(const boost::python::object &arg) const
	{
		PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("Looking up overload", arg);
		if (arg.is_none())
		{
			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Looking up overload with no parameters...");
			return GetSpecificOverload(boost::python::tuple());
		}
		else
		{
			boost::python::extract<int> maybeIndex(arg);
			if (maybeIndex.check())
			{
				int index = maybeIndex;
				PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("Getting overload at position ", index);

				if (index < 0 || index >= (int)_invokers.size())
				{
					throw_index_error();
					throw std::runtime_error("Index out of range");
				}

				return _invokers[index];
			}

			boost::python::extract<boost::python::tuple> maybeTuple(arg);
			if (maybeTuple.check())
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Looking up overload with multiple parameters...");
				return GetSpecificOverload(maybeTuple);
			}
			else
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Looking up overload with single parameter...");
				boost::python::list lst;
				lst.append(arg);
				return GetSpecificOverload(boost::python::tuple(lst));
			}
		}
	}

	template<typename InvokerType>
	InvokerType DynamicOverloadResolver<InvokerType>::GetSpecificOverload(const boost::python::tuple &args) const
	{
		PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("GetSpecificOverload", args);

		const int nArgs = boost::python::len(args);
		const int nInvokers = _invokers.size();

		for (int i = 0; i != nInvokers; ++i)
		{
			array<ParameterInfo ^> ^pis = _invokers[i].GetParameters();

			if (pis->Length != nArgs)
			{
				continue;
			}

			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG(("Checking overload: " + _invokers[i].GetSignature()));

			bool notMatch = false;

			for (int j = 0; j != nArgs; ++j)
			{
				boost::python::object arg = args[j];
				System::Type ^paramType = pis[j]->ParameterType;
				
				if (paramType->IsGenericParameter)
				{
					continue;
				}

				if (paramType->IsByRef)
				{
					paramType = paramType->GetElementType();
				}

				boost::python::extract<const DynamicTypeHandle &> maybeType(arg);
				if (!maybeType.check())
				{
					arg = arg.attr("__class__");
					maybeType = boost::python::extract<const DynamicTypeHandle &>(arg);
				}

				if (maybeType.check())
				{
					const DynamicTypeHandle &typeArg = maybeType;

					auto typeArgObject = typeArg.GetTypeObject();

					if (! paramType->Equals(typeArgObject) )
					{
						PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG(
							(std::string("Did not match signature at position: ") + 
							boost::lexical_cast<std::string>(j)));

						notMatch = true;
						break;
					}
				}
				else
				{
					throw_exception("Type expected");
					throw std::runtime_error("Type expected");
				}
			}

			if (!notMatch)
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG(("Using overload: " + _invokers[i].GetSignature()));
				return _invokers[i];
			}
		}

		throw_exception("No such overload");
		throw std::runtime_error("No such overload");
	}

	template<typename InvokerType>
	int DynamicOverloadResolver<InvokerType>::FindSuitableOverloadIndex(const boost::python::tuple &args) const
	{
		PYDOTNET_DYNAMICMETHOD_PRINT_WITH_ARGS_DEBUG("FindSuitableOverloadIndex", args);

		const int nArgs = boost::python::len(args);
		const int nInvokers = _invokers.size();

		for (int i = 0; i != nInvokers; ++i)
		{
			array<ParameterInfo ^> ^pis = _invokers[i].GetParameters();

			if (pis->Length != nArgs)
			{
				continue;
			}

			PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG(("Checking overload: " + _invokers[i].GetSignature()));

			bool notMatch = false;

			for (int j = 0; j != nArgs; ++j)
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG(
					(std::string("Testing at position: ") + boost::lexical_cast<std::string>(j)));

				boost::python::object arg = args[j];
				System::Type ^paramType = pis[j]->ParameterType;

				if (paramType->IsGenericParameter)
				{
					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Generic");
					continue;
				}
				if (paramType->IsPrimitive)
				{
					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Primitive");
					if (boost::python::extract<int>(arg).check() || boost::python::extract<double>(arg).check())
					{
						continue;
					}
				}
				else if (arg.is_none())
				{
					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("nullptr");
					continue;
				}
				else if (paramType->Equals(System::String::typeid))
				{
					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("String");
					if (boost::python::extract<std::string>(arg).check()) 
					{ 
						continue; 
					}
				}
				else
				{
					PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("Class / Interface");
					boost::python::extract<const DynamicObjectHandle &> maybeObject(arg);
					if (maybeObject.check())
					{
						const DynamicObjectHandle &objectArg = maybeObject;

						if (paramType->IsAssignableFrom(objectArg.GetTypeObject()) ) 
						{ 
							continue; 
						}
					}
					else if (boost::python::extract<boost::python::list>(arg).check())
					{
						PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG("List");
						if (paramType->IsArray)
						{
							continue;
						}
						else if (paramType->IsGenericType)
						{
							auto genericArgs = paramType->GetGenericArguments();
							if (genericArgs->Length == 1)
							{
								auto enumerable = IEnumerable<System::Object ^>::typeid->
									GetGenericTypeDefinition()->MakeGenericType(genericArgs[0]);

								if (enumerable->IsAssignableFrom(paramType))
								{
									continue;
								}
							}
						}
					}
					if (System::Delegate::typeid->IsAssignableFrom(paramType))
					{
						if (hasattr(arg, "__call__"))
						{
							continue;
						}
					}
				} 

				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG(
					(std::string("Did not match signature at position: ") + boost::lexical_cast<std::string>(j)));

				notMatch = true;
				break;
			}

			if (!notMatch)
			{
				PYDOTNET_DYNAMICMETHOD_PRINT_DEBUG(("Using overload: " + _invokers[i].GetSignature()));
				return i;
			}
		}

		return -1;
	}

	void DynamicMethodInvoker::RegisterOverloads(const std::string &name)
	{
		DynamicOverloadResolver<DynamicMethodInvoker>::Register(name);
	}

	void DynamicConstructorInvoker::RegisterOverloads(const std::string &name)
	{
		DynamicOverloadResolver<DynamicConstructorInvoker>::Register(name);
	}

}// namespace InteropPython
