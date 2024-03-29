# Python vs. dotNET Interoperability

**NOTE** *This is Markdown copy of these [Python Notebook Slides](https://github.com/sadhbh-c0d3/pydotnet/tree/main/slides)*

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

## Shadowed builtins integrated into __main__ when importing dotnet.seamless
There are built-in functions are defined in dotnet.overrides and they are:

    # Print all from `dotnet.overrides` that were integrated into `__main__` by `dotnet.seamless`
    print(pretty_names(x for x in dir() if x in dotnet.overrides.__all__))
    
    help
    isinstance, issubclass
    type

**NOTE** They call original built-in functions when used with Python types.

## Import .NET type into Python namespace
We can import .NET namespace just like if it was Python module:

    import System
    System.Int32

    <class Int32>

And we can import specific symbols from .NET namespace into current Python scope:

    from System.Collections.Generic import List
    List

    <class List`1>

## Managed types available by default
Certain managed types, which map to Python types are available from start

    System: Void, Object, String
    Numeric: Int16, Int32, Int64, UInt16, UInt32, UInt64, Byte, SByte, Single, Double, Decimal,
    Collections: Array[T], List[T], Dictionary[K, V],
    Tuples: Tuple1[T1], Tuple2[T1, T2], ..., Tuple8[T1, T2,..., T8],
    Actions: Action1[T1], Action2[T1, T2], ..., Action8[T1, T2,..., T8],
    Functions: Func1[T1], Func2[T1, T2], ..., Func8[T1, T2,..., T8],

**NOTE** These types are imported by dotnet.commontypes. 
Some of these types are defined within dotnet.proxytypes, and 
because of that we should use these and not try to import ones from System.

    # Example types
    String, Int32, Tuple2[String, Int32]

`   (<class String>, <class Int32>, <class Tuple`2>)
 
### Built-in Type Conversions
Conversion of Python values into .NET method arguments
When calling .NET method the parameters are converted from Python to .NET depending on managed method signature:

- Any parameter accepts Python None as .NET null
- Any parameter of integer type (e.g. Int16, Int32, Int64) accepts Python int or long
- Any parameter of floating point type (e.g. Single, Double) accepts Python float
- Any parameter of String type accepts Python str
- Any parameter of IEnumerable type accepts Python list or .NET IEnumerable
- Any parameter of IDictionary type accepts Python dict or .NET IDictionary
- Any parameter of Action<> type accepts Python function or .NET Action<>
- Any parameter of Func<> type accepts Python function or .NET Func<>
- Any parameter of other Delegate type accepts Python function or matching .NET Delegate

All .NET objects are represented by PyDotnet.Interop.Object.

When Python function get converted to .NET Action<>, Func<>, or Delegate there is assumption that number of parameters of the Python function matches target Action<>, Func<>, or Delegate.

#### Example
    lst = List[Int32]()

    # Python `int` gets converted into `Int32`
    lst.Add(1)
    lst.Add(2)
    lst.Add(3)

    # Python `list` gets converted into `List<Int32>`
    lst.AddRange([4,5,6])
    lst

    [1, 2, 3, 4, 5, 6]

### Conversion of .NET method return values into Python
When calling .NET method the returned value is converted to Python:

- All .NET integer types (e.g. Int16, Int32, Int64) convert to either int or long
- All .NET floating point types (e.g. Single, Double) convert always to float
- Any .NET String always converts to str
- Any .NET null always converts to None
- Any other .NET types are not converted and PyDotnet.Interop.Object is used to represent them in Python

#### Example
    lst = List[Int32]([1,2,3,4,5,6])

    # Python `lambda` gets converted into `System.Predicate<Int32>`
    lst.FindIndex(lambda x: x > 3)

    3
### Conversion of Python values into .NET method arguments of type System.Object
When .NET method argument is of type System.Object it can accept any type.

The dotnet module provides automatic conversions:

- Any int or long is converted to Int32 or Int64
- Any float is converted to Double
- Any str is converted to String

        # We construct a list of `System.Object`, which even includes `List<Int32>` or `Action<System.Object>`
        lst = List[Object]([1, 2.5, List[Int32]([1,2,3]), Action1[Object](lambda x: 5)])

        print('Items', repr(lst))
        print('Types', map(type, lst))
        Items [1, 2.5, [1, 2, 3], <Action`1 instance>]
        Types [<type 'int'>, <type 'float'>, <List`1 type 'instance'>, <Action`1 type 'instance'>]

### Explicit type conversion of parameters
Sometimes it is not possible to get correct automated guess to what type Python value should be converted. It is possible to explicitly specify parameter types.

    z = List[Object]()

    # Calling `Add()` method while specifying exact parameter type
    z.Add[Int32](1)
    z.Add[Double](1)
    z.Add(None)
    z.Add[String]('Hello')

    map(type, z)

    [int, float, NoneType, str]

When we call help(z.Add) we'll see `Add(item: Object)` signature

    help(z.Add)

Help on method `List[Object]`.Add in module mscorlib:
  
      Add(item: Object)

    References:
     |
     |  ----------------------------------------------------------------------
     |  Types defined in assembly mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:
     |
     |
     |    within namespace System.Collections.Generic:
    ...

But when we call `help(z.Add[Double])` we'll see `Add(item: Double)` signature

    help(z.Add[Double])
    
Help on method `List[Object].Add` in module `mscorlib`:
  
    Add(item: Double)
  

    References:
     |
     |  ----------------------------------------------------------------------
     |  Types defined in assembly mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:
     |
     |
     |    within namespace System:
    ...

## Collections
### Array Type `T[]`
An array type `T` can be created using `Array[T]`

    a = Array[Int32]([1,2,3,4])

    print(repr(a), ':', type(a))
    [1, 2, 3, 4] : System.Int32[]
List Type `List<T>`
A `List[T]` can be used to store a sequence of `T` elements.

    b = List[Int32]([1,2,3,4])

    print(repr(b), ':', type(b))
    [1, 2, 3, 4] : System.Collections.Generic.List`1[System.Int32]
    
### Dictionary Type `Dictionary<K,V>`
A `Dictionary[K,V]` can be used to create `K => V` mapping

    c = Dictionary[String, Int32]({'a':10, 'b':20, 'c':30})

    print(repr(c), ':', type(c))
    {'a': 10, 'c': 30, 'b': 20} : System.Collections.Generic.Dictionary`2[System.String,System.Int32]

## Type of .NET object
We can use `type(x)` to see what is the type of our .NET object `x`.

The `type()` function is imported to `__main__` and shadows built-in function when we `import dotnet.seamless`.
Otherwise it's available in `dotnet.overrides module`.

        # Let's see what will be the type for Array[Int32]
        print('dotNET Type:', type(a))
        print('Python type:', builtins.type(a))
        
        dotNET Type: System.Int32[]
        Python type: <class 'dotnet.PyDotnet.Object'>

        # Let's see what will be the type for List[Int32]
        print('dotNET Type:', type(b))
        print('Python type:', builtins.type(b))

        dotNET Type: System.Collections.Generic.List`1[System.Int32]
        Python type: <class 'dotnet.PyDotnet.Object'>

The overriden `type()` function only changes behavior for .NET objects, and works as always for Python objects

        type(1), type('Hello World!'), type([1, 2, 3]), type({'a':1, 'b':2})

        (int, str, list, dict)

## Help system
The `help()` function is imported to main and shadows built-in function when we import `dotnet.seamless`. 
Otherwise it's available in `dotnet.overrides module`.

We can use `help(x)` to see help for `x`, which can be any of .NET:

- namespace, e.g. `help(System)`
- class, e.g. `help(System.Int32)`
- object, e.g. `help(List[Int32]())`
- method, e.g. `help(List[Int32]().Add)`
- method overloads, e.g. `help(List[Int32]().FindIndex)`
- constructors, e.g. `help(List[Int32].__createinstance__)`
- 
**NOTE** The `help(dotnet.clr)` gives list of all namespaces and all loaded assemblies.

### Help on namespace

    help(System.Collections)

Help on namespace System.Collections:

    namespace System.Collections
     |
     |  Data and other attributes defined here:
     |
     |  class ArrayList
     |  class ArrayListDebugView
     |  class BitArray
     |  class CaseInsensitiveComparer
     |  class CaseInsensitiveHashCodeProvider
     |  class CollectionBase
    ...

### Help on method

    help(List[Int32].Add)
    
Help on method `List[Int32].Add` in module `mscorlib`:
  
    Add(item: Int32)

    References:
     |
     |  ----------------------------------------------------------------------
     |  Types defined in assembly mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:
     |
     |
     |    within namespace System.Collections.Generic:
    ...


### Help on constructors

    help(List[Int32].__createinstance__)
    
Help on method `List[Int32].__init__` in module `mscorlib`:
  
      __init__() -> List[Int32]

      __init__(capacity: Int32) -> List[Int32]

      __init__(collection: IEnumerable[Int32]) -> List[Int32]


    References:
     |
     |  ----------------------------------------------------------------------
    ...

## Assemblies and Namespaces
### Assembly Injection
In later example we will want to load some C# assembly, so we start with an example of how we can actually create an assembly directly from Python code!

We will build some assembly containing some example classes. It will go to `C:\Temp\PyDotnet` folder in our case.

    # And here comes C# source-code
    source = """
    using System;
    using System.Collections.Generic;

    namespace Beach {
    namespace Sea {
    namespace Ships {

    public interface IShip {
        string Name { get; }
        object Payload { get; set; }
    }

    public class Frigate : IShip {
        private readonly string m_name;

        public Frigate(string name) {
            Console.WriteLine("Creating Frigate: {0}", name);
            m_name = name;
        }

        public string Name { get { return m_name; } }

        public object Payload { get; set; }
    }

    public static class Ranges {
        private static IDictionary<string, int> m_ranges;

        public static void SetRanges(IDictionary<string, int> ranges) {
            Console.WriteLine("Setting ranges: {0}", ranges);
            m_ranges = ranges;
        }

        public static int GetRange(this IShip ship) {
            if (m_ranges == null)
                return -1;
            Type shipType = ship.GetType();
            int range;
            if (m_ranges.TryGetValue(shipType.Name, out range)) {
                return range;
            }
            return -1;
        }
    }

    public static class ShipExtensions {
        public static void AddPayload<TPayload>(this IShip ship, TPayload payload) {
            ship.Payload = payload;
        }
    }

    }
    }
    }
    """


    import os

    asmpath = r'C:\Temp\PyDotnet'

    if not os.path.isdir(asmpath):
        os.mkdir(asmpath)   

    output = os.path.join(asmpath, 'Beach.Sea.dll')

    # Need to delete previous one (if any)
    if os.path.isfile(output):
        os.remove(output)

    # Need to specify references
    references = ['mscorlib.dll']

    # Let's build
    dotnet.build_assembly(source, output, references)


### Assemblies
.NET assemblies can be loaded using `load_assembly()`, but first `add_assemblies()` need to be used to point to assemblies location.

    dotnet.add_assemblies(r'C:\\Temp\\PyDotnet')
    
**NOTE** The `add_assemblies()` can be called multiple times to add multiple locations, 
and `load_assembly()` will use FIFO priority.

    # We can use `help(clr)` to see list of namespaces and assemblies, or we can use `assemblies(filter)`
    names = dotnet.assemblies('Beach')

    print(pretty_names(names, 10, 1))
    Beach.Sea

### Mutiple assemblies can be loaded at once using pattern matching.

    dotnet.load_assemblies('Beach')

We can obtain list of loaded assemblies with call to loaded_assemblies()

    # Let's see what 'Beach' assemblies were loaded
    names = set(x.FullName for x in dotnet.loaded_assemblies('Beach'))

    print(pretty_names(names, 10, 1))
    Beach.Sea, Version=0.0.0.0, Culture=neutral, PublicKeyToken=null

### Namespaces
We can see available namespaces with call to namespaces()

**NOTE** By supplying parameter list will be filtered.

    # Let's see what are the namespaces containing 'Beach' word
    names = dotnet.namespaces('Beach')

    print(pretty_names(names, 12, 1))
    Beach
    Beach.Sea
    Beach.Sea.Ships

### Accessing types defined within namespaces
We can access types in those namespaces via `clr`

    # We can use `help()` to see what types are defined in the namespace
    help(dotnet.clr.Beach.Sea.Ships)
    Help on namespace Beach.Sea.Ships:

    namespace Beach.Sea.Ships
     |
     |  Data and other attributes defined here:
     |
     |  class Frigate
     |  class IShip
     |  class Ranges
     |  class ShipExtensions

We can also access types in those namespaces by using import statement

    # We can also use `import`
    import Beach.Sea

    # We can also use `from * import *`
    from Beach.Sea.Ships import Frigate

    # The `help()` can be used practically on anything
    help(Beach.Sea.Ships.Ranges.SetRanges)

Help on method Ranges.SetRanges in module Beach.Sea:
  
          static SetRanges(ranges: IDictionary[String, Int32])


        References:
         |
         |  ----------------------------------------------------------------------
         |  Types defined in assembly mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:
         |
         |
         |    within namespace System:
        ...

## String representation of .NET object
Python defines two functions `str()` and `repr()`.

The implementation of `str()` for .NET objects calls `ToString()`
The implementation of `repr()` for .NET objects creates Python compatible representation string

    x = List[Int32]([1,2,3,4])

    print('repr(): ' + repr(x) + ', str(): ' + str(x))
    repr(): [1, 2, 3, 4], str(): System.Collections.Generic.List`1[System.Int32]

### Pretty printing
In addition to `str()` and `repr()` new function `pretty()` has been added for .NET objects.

The `pretty()` function prints out the properties of the object, or multiple objects.

    from System import TimeSpan, DateTime
    from dotnet import pretty

    a = TimeSpan(10,0,0)
    b = DateTime(2016,1,1)

    print(pretty([a, b]))
    TimeSpan:
        Ticks:	360000000000
        Days:	0
        Hours:	10
        Milliseconds:	0
        Minutes:	0
        Seconds:	0
        TotalDays:	0.416666666667
        TotalHours:	10.0
        TotalMilliseconds:	36000000.0
        TotalMinutes:	600.0
        TotalSeconds:	36000.0
    DateTime:
        Date:	instance of DateTime
        Day:	1
        DayOfWeek:	instance of DayOfWeek
        DayOfYear:	1
        Hour:	0
        Kind:	instance of DateTimeKind
        Millisecond:	0
        Minute:	0
        Month:	1
        Second:	0
        Ticks:	635872032000000000
        TimeOfDay:	instance of TimeSpan
        Year:	2016

**NOTE** The main purpose of pretty() function is to use it with interactive Python shell.

## Methods and Constructors
### Method Overloads
Automatic resolution of method overloads is supported.

    x = List[Int32]([1,2,3,4])

    # Let's use `FindIndex(Int32 startIndex, Predicate<Int32> match)`
    x.FindIndex(0, lambda a: a > 3)

    3

    help(x.FindIndex)
    
Help on method `List[Int32].FindIndex` in module `mscorlib`:
  
      FindIndex(match: Predicate[Int32]) -> Int32

      FindIndex(startIndex: Int32, match: Predicate[Int32]) -> Int32

      FindIndex(startIndex: Int32, count: Int32, match: Predicate[Int32]) -> Int32


    References:
     |
     |  ----------------------------------------------------------------------
    ...

### Explicit method overload selection
Sometimes automatic method overload resolution doesn't work as expected. We can still specify overload explicitly.

**NOTE** None can be used to select parameterless overload.

    x = List[Int32]([1,2,3,4])

    # Let's use `FindIndex(Int32 startIndex, Predicate<Int32> match)`
    x.FindIndex[Int32, System.Predicate[Int32]](0, lambda a: a > 3)

    3

    # Explicit overload selection
    help(x.FindIndex[Int32, System.Predicate[Int32]])
    Help on method List[Int32].FindIndex in module mscorlib:

      FindIndex(startIndex: Int32, match: Predicate[Int32]) -> Int32


    References:
     |
     |  ----------------------------------------------------------------------
     |  Types defined in assembly mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:
     |
     |
     |    within namespace System.Collections.Generic:
    ...

### Constructors
The constructors can be accessed via __createinstance__ property, and it may represent constructor overloads.


    # Let's see how can we construct a List of Int32
    help(List[Int32].__createinstance__)

Help on method `List[Int32].__init__` in module `mscorlib`:

      __init__() -> List[Int32]

      __init__(capacity: Int32) -> List[Int32]

      __init__(collection: IEnumerable[Int32]) -> List[Int32]


    References:
     |
     |  ----------------------------------------------------------------------
    ...

### Explicit constructor selection
Sometimes automatic constructor resolution doesn't work as expected. We can still specify overload explicitly.

**NOTE** None can be used to select parameterless constructor.

    # Let's select first constructor (the parameterless one)
    help(List[Int32][None])
    Help on method List[Int32].__init__ in module mscorlib:

      __init__() -> List[Int32]


    References:
     |
     |  ----------------------------------------------------------------------
     |  Types defined in assembly mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089:
     |
     |
     |    within namespace System.Collections.Generic:
    ...

### Generic methods
The generic methods are supported seamlessly.

    from Beach.Sea.Ships import Frigate, ShipExtensions

    frigate = Frigate('DaVinci')

    payload = List[Int32]([1,2,3])

    # We call generic method as any other method
    ShipExtensions.AddPayload(frigate, payload)

    frigate.Payload
    [1, 2, 3]

    # Let's take a look at `Fire` method
    help(ShipExtensions.AddPayload)
    Help on method ShipExtensions.AddPayload in module Beach.Sea:

      static AddPayload(ship: IShip, payload: TPayload)


    References:
     |
     |  ----------------------------------------------------------------------
     |  Types defined in assembly Beach.Sea, Version=0.0.0.0, Culture=neutral, PublicKeyToken=null:
     |
     |
     |    within namespace Beach.Sea.Ships:
    ...

### Explicit generic parameter types specialization
Sometimes automatic generic parameter type resolution doesn't work. In those cases we can still select specialization that we want to use.

    In [44]:
    # Explicit generic parameter types specialization
    ShipExtensions.AddPayload[List[Int32]](frigate, [1,2,3])

**Note** that since we explicilty say that AddPayload method takes a List<Int32> we can pass python list, and it will implicitly get converted into List<int32>, because this is the expected type.


## Extension methods
The extension methods are supported seamlessly.

    frigate = Frigate("DaVinci")

    Beach.Sea.Ships.Ranges.SetRanges({'Frigate':2000})

    # Calling extension method GetRange()
    frigate.GetRange()
    2000

We can also call any generic extension method:

    frigate = Frigate("DaVinci")

    payload = List[Int32]([1,2,3])

    # We can also call generic extension method
    frigate.AddPayload(payload)
    def f(x):
        print('x = ', x)

    # We can wrap any python function into either Action or Func
    payload = Action1[Int32](f)

    frigate.AddPayload(payload)

    frigate.Payload.Invoke(1)
    x =  1

Should the extension method be generic, the type parameters can be specialized explicitly.

    frigate.AddPayload[List[Int32]]([1,2,3])

    frigate.Payload
    [1, 2, 3]


