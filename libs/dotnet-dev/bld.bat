if not defined PYTHON (
    set PYTHON=python
)

echo Using PYTHON=%PYTHON%

if not defined CONDA_BUILD (

    for /f %%a in ('%PYTHON% -c "import sys; print(sys.prefix)"') do set PREFIX=%%a
    if errorlevel 1 exit /b 1

    for /f %%b in ('%PYTHON% -c "import sys; print(\"{0.major}.{0.minor}\".format(sys.version_info))"') do set PY_VER=%%b
    if errorlevel 1 exit /b 1

    set RECIPE_DIR=%~dp0

    echo Using RECIPE_DIR=%RECIPE_DIR%

    REM Download and extract boost
    if exist "..\dotnet-dev" (
        rem --- Then we are running bld.bat from where it's located ---

        if not exist ".\boost_1_64_0" (
            rem --- Let's download boost 1.64 ---

            rem === The nice_download.py and nice_extract_targz.py show progress, while download.py doesn't ===
            rem === However nice_download.py may not work on PyPy as it uses urllib2 ===
            rem === Let's move that to 'download_boost.bat' maybe? ===

            if not exist ".\boost_1_64_0.tar.gz" (
                %PYTHON% "%RECIPE_DIR%nice_download.py" https://boostorg.jfrog.io/ui/native/main/release/1.64.0/source/boost_1_64_0.tar.gz
                if errorlevel 1 exit /b 1
            )

            rem For some reason error occurs when extracting one of the html docs. We don't really need them anyways
            %PYTHON% "%RECIPE_DIR%nice_extract_targz.py" boost_1_64_0.tar.gz doc/html
            if errorlevel 1 exit /b 1
        )

        cd boost_1_64_0
        if errorlevel 1 exit /b 1
    )

)

rem -- exit /b 1

if not exist ".\boost\version.hpp" (
    echo Need to run this from boost root folder.
    exit /b 1
)

%PYTHON% -c "fp = open('boost\\version.hpp');exit(0 if '1_64' in fp.read() else 1);fp.close()"
if errorlevel 1 (
    echo Boost version 1.64 expected
    exit /b 1
)

REM Get the architecture of the python interpreter
for /f %%c in ('%PYTHON% -c "import os; print(os.environ[\"PROCESSOR_ARCHITECTURE\"])"') do set ARCH=%%c
if errorlevel 1 exit /b 1

for /f %%d in ('%PYTHON% -c "import platform; print(platform.architecture()[0][:2])"') do set BITNESS=%%d
if errorlevel 1 exit /b 1

echo Using ARCH=%ARCH%
echo Using BITNESS=%BITNESS%

if not defined VisualStudioVersion (
    if defined VS150COMNTOOLS (
        call "%VS150COMNTOOLS%\..\..\VC\Auxiliary\Build\vcvarsall.bat" %ARCH%
    ) else (
       if defined VS140COMNTOOLS (
           call "%VS140COMNTOOLS%\..\..\VC\vcvarsall.bat" %ARCH%
       ) else (
           call "%VS110COMNTOOLS%\..\..\VC\vcvarsall.bat" %ARCH%
       )
    )
    if errorlevel 1 exit /b 2
)

echo Using VisualStudioVersion=%VisualStudioVersion%


set MSSdk=1
set DISTUTILS_USE_SDK=1

REM Build
call bootstrap
if errorlevel 1 exit /b 3

set JAMCONF=%HOMEDRIVE%%HOMEPATH%\user-config.jam

copy /Y tools\build\example\user-config.jam %JAMCONF%
if errorlevel 1 exit /b 4

%PYTHON% "%RECIPE_DIR%\configure.py" >> %JAMCONF%
if errorlevel 1 exit /b 4

echo Contents of %JAMCONF%
type %JAMCONF%

b2 --with-python stage toolset=msvc-%VisualStudioVersion% variant=release link=shared threading=multi runtime-link=shared address-model=%BITNESS% cxxflags=/Gd -a
if errorlevel 1 exit /b 5
b2 --with-regex stage toolset=msvc-%VisualStudioVersion% variant=release link=static threading=multi runtime-link=shared address-model=%BITNESS% cxxflags=/Gd define=BOOST_REGEX_NO_FASTCALL -a
if errorlevel 1 exit /b 5


REM Install
xcopy stage\lib\*.lib %PREFIX%\Library\lib  /y /i /s
if errorlevel 1 exit /b 6
xcopy stage\lib\*.dll %PREFIX%\Library\lib  /y /i /s
if errorlevel 1 exit /b 6
xcopy stage\lib\*.dll %PREFIX%\Scripts      /y /i /s
if errorlevel 1 exit /b 6
xcopy boost %PREFIX%\Library\include\boost  /y /i /s /q
if errorlevel 1 exit /b 6
