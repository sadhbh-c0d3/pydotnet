# PyDotNET (pip: dotnet)
(CPython) &lt;==> (.NET) direct interop via embedded CLR (.pyd) through Boost.Python

**NOTE** Works only on Windows due to integration with Microsoft .NET environment

## Usage

#### Example

    import dotnet.seamless
    import System
    
    from System.Collections.Generic import List
    
    lst = List[Int32]()
    
    lst.Add(1)
    lst.Add(2)
    lst.Add(3)
    
    lst.AddRange([4,5,6])
    
    lst.FindIndex(lambda x: x > 3)

#### Explanation

Import .NET seamless integration for CPython shell

    import dotnet.seamless
    
Import .NET namespace

    import System
    
Import type from .NET namespace

    from System.Collections.Generic import List
    
Create an instance of .NET type.
This shows also how to specialize .NET generic type

    lst = List[Int32]()

Call instance method of .NET type. 
This also converts Python int into .NET Int32

    lst.Add(1)
    lst.Add(2)
    lst.Add(3)
    
Call instance method of .NET type. 
This also converts Python list into .NET IEnumerable

    lst.AddRange([4,5,6])

Call instance method of .NET type.
Here Python lambda gets converted into .NET Predicate<Int32>.
The invocation jumps from Python into .NET CLR and calls back to Python lambda

    lst.FindIndex(lambda x: x > 3)





## Installation

    pip install dotnet

## Documentation

See https://github.com/sadhbh-c0d3/pydotnet/blob/organize-wiki/SLIDES.md

## Building

See https://github.com/sadhbh-c0d3/pydotnet/blob/organize-wiki/BUILD.md
