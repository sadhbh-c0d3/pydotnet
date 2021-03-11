
REM Install all runtime deps side-by-side for a production release
REM (in this way dotnet won't need dotnet-dev at runtime)
xcopy %LIBRARY_LIB%\boost_python*.dll dotnet /y /i /s
if errorlevel 1 exit 3 /b

REM Build and install the library
"%PYTHON%" setup.py install
if errorlevel 1 exit 5 /b
