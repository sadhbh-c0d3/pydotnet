echo Environment
set

if not defined PYTHON (
    set PYTHON=python
)

%PYTHON% ./setup.py build
%PYTHON% ./setup.py install
%PYTHON% ./setup.py test --test-suite dotnet.test
%PYTHON% ./setup.py bdist_wheel
