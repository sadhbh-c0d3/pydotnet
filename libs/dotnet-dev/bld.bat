@echo Environment
@set

@if not defined PYTHON (
    set PYTHON=python
)

@echo Using PYTHON=%PYTHON%

@if not defined CONDA_BUILD (
    for /f %%a in ('%PYTHON% -c "import sys; print(sys.prefix)"') do set PREFIX=%%a
    if errorlevel 1 exit /b 1

    for /f %%b in ('%PYTHON% -c "import sys; print(\"{0.major}.{0.minor}\".format(sys.version_info))"') do set PY_VER=%%b
    if errorlevel 1 exit /b 1

    set RECIPE_DIR=%~dp0

    echo Using RECIPE_DIR=%RECIPE_DIR%

    if exist "..\dotnet-dev" (
        rem --- Then we are running bld.bat from where it's located ---

        if not exist ".\boost_1_84_0" (
            REM Download and extract boost
            %PYTHON% "%RECIPE_DIR%get_boost.py" "1.84.0"
            if errorlevel 1 exit /b 1
        )

        cd boost_1_84_0
        if errorlevel 1 exit /b 1
    )

)

@rem -- exit /b 1

@if not exist ".\boost\version.hpp" (
    echo Need to run this from boost root folder.
    exit /b 1
)

@%PYTHON% -c "fp = open('boost\\version.hpp');exit(0 if '1_84' in fp.read() else 1);fp.close()"
@if errorlevel 1 (
    echo Boost version 1.84 expected
    exit /b 1
)

@for /f %%d in ('%PYTHON% -c "import platform; print(platform.architecture()[0][:2])"') do set BITNESS=%%d
@if errorlevel 1 exit /b 1

@echo Using ARCH=%ARCH%
@echo Using BITNESS=%BITNESS%

@if not defined VisualStudioVersion (
    echo Need to run this from Developer Command Prompt for Visual Studio.
    exit /b 3
)

@set MSSdk=1
@set DISTUTILS_USE_SDK=1

@echo MSVC %VisualStudioVersion% %BITNESS%
@echo Python: %PY_VER% : %PREFIX%

@echo Bootstrapping Boost
@call bootstrap
@if errorlevel 1 exit /b 3

@set JAMCONF=%HOMEDRIVE%%HOMEPATH%\user-config.jam

@copy /Y tools\build\example\user-config.jam %JAMCONF%
@if errorlevel 1 exit /b 4

@%PYTHON% "%RECIPE_DIR%\configure.py" >> %JAMCONF%
@if errorlevel 1 exit /b 4

@echo Contents of %JAMCONF%
@type %JAMCONF%

@echo Building Boost.Python : %BITNESS%-bit
@b2 --with-python stage toolset=msvc variant=release link=shared threading=multi runtime-link=shared architecture=x86 address-model=%BITNESS% cxxflags=/Gd -a
@if errorlevel 1 exit /b 5

@echo Fixing msvc version in filenames
%PYTHON% "%RECIPE_DIR%\msvcverfix.py" stage\lib

@echo Staged libs
@dir stage\lib

@echo Install boost libraries into %PREFIX%
@xcopy stage\lib\*.lib %PREFIX%\Library\lib  /y /i /s
@if errorlevel 1 exit /b 6
@xcopy stage\lib\*.dll %PREFIX%\Library\lib  /y /i /s
@if errorlevel 1 exit /b 6
@xcopy stage\lib\*.dll %PREFIX%\Scripts      /y /i /s
@if errorlevel 1 exit /b 6
@xcopy boost %PREFIX%\Library\include\boost  /y /i /s /q
@if errorlevel 1 exit /b 6

@echo Build environment ready. Please run "python setup.py build" now.

