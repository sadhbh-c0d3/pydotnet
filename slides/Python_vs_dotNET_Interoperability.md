# Python vs. dotNET Interoperability

## Getting Started

Import *dotnet* module

    import dotnet.seamless

**NOTE** The `dotnet.seamless` module is tailored to provide seamless Python integration with .NET. 
When importing dotnet (and not dotnet.seamless) the support for generic and extension methods is not installed, 
basic .NET types and built-in function overrides are not integrated into `__main__`.

## Loading Assemblies
The assembly related functions are:

    print(pretty_names(x for x in dir(dotnet) if x in dotnet.asmresolve.__all__))

    add_assemblies
    assemblies
    load_assemblies, load_assembly
    set_assemblies

**NOTE** Use `add_assemblies()` to add path where your .NET assemblies are located, and 
use `load_assembly()` or `load_assemblies()` to load.

## .NET types integrated into __main__ when importing dotnet.seamless
Certain basic .NET types are automatically imported into __main__:

    # Print all from `dotnet.commontypes` that were integrated into `__main__` by `dotnet.seamless`
    print(pretty_names((x for x in dir() if x in dir(dotnet.commontypes)), 1, 8))
    Action1, Action2, Action3, Action4, Action5, Action6, Action7, Action8
    Array
    Byte
    Decimal, Dictionary, Double
    Func1, Func2, Func3, Func4, Func5, Func6, Func7, Func8
    Int16, Int32, Int64
    List
    Object
    SByte, Single, String
    Tuple1, Tuple2, Tuple3, Tuple4, Tuple5, Tuple6, Tuple7, Tuple8
    Type
    UInt16, UInt32, UInt64
    Void

    dotnet
    
**NOTE** All built-in Python types are in lower-case and .NET types are in CamelCase.
e.g. So when you see Int32 or String it's a .NET type, and if you see int or str it's Python type.

