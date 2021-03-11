"""
Provides essential build functionality if pps is not available.
"""
import os
import re
import glob
import setuptools
import subprocess

from distutils import log
from distutils.errors import DistutilsModuleError


class VersionFileError(DistutilsModuleError):
    pass


class Command(setuptools.Command):

    @staticmethod
    def find_commands():
        """
        Get all default pps.build commands. Return a dictionary of command names
        mapping to their corresponding Command classes.

        Find all setup commands and overwrite their __name__-s to match the
        command names. This is necessary to prevent setuptools from listing the
        raw class names in the help page. Note that __subclasses__ can't be used
        here because setuptools Command-s are old-style classes on Python 2.

        @return: a map of the form: command name => Command class
        @rtype: dict
        """
        commands = {}

        for klass in dict(globals()).values():
            try:
                if issubclass(klass, Command) and klass is not Command:
                    assert klass.COMMAND not in commands
                    klass.__name__ = klass.COMMAND

                    commands[klass.COMMAND] = klass
            except:
                continue

        return commands

    @staticmethod
    def use_toolset(vcvars):
        """
        Override the automatic toolset discovery mechanism in distutils
        and force it to compile extensions using the specified MSVC toolset.

        The specified toolset path can contain environment variables. For
        example, a handy way to specify MSVC 11.0 is::

            "%VS110COMNTOOLS%\..\..\VC\vcvarsall.bat"

        @param vcvars: full path to C{vcvarsall.bat}, used to initialize
                       the toolset
        @type  vcvars: str
        """
        from distutils import msvc9compiler

        plat = msvc9compiler.PLAT_TO_VCVARS[msvc9compiler.get_platform()]
        c = subprocess.check_output('call "{0}" {1} && set'.format(vcvars, plat), shell=True)

        for line in c.decode().splitlines():
            var, sep, value = line.partition("=")
            os.environ[str(var)] = str(value)

        os.environ["MSSdk"] = "1"
        os.environ["DISTUTILS_USE_SDK"] = "1"


class VersionResolver(Command):
    """
    Locate the verison.py file in the current project and finalize its
    {BUILD} and {REVISION} macros (this overwrites the file in place).
    If no build number is provided, don't touch version.py, just announce
    the existing version.

    The version file is expected to be called version.py and located under
    the root namespace: "root/verison.py". It must contain a semantic version
    string in a C{__version__} variable. It can also contain an optional
    C{__revision__} variable::

        __version__ = "0.2.3.{BUILD}"
        __revision__ = "{REVISION}"

    A new TeamCity message will be published to announce the finalized
    version number.
    """
    COMMAND = "finalize"
    VERSION_FILE = "version.py"
    PROJECT = "."

    VERSION = "__version__"
    REVISION = "__revision__"

    BUILD_MACRO = "{BUILD}"
    REV_MACRO = "{REVISION}"

    TC_MESSAGE = """##teamcity[buildNumber '{version.full}']
##teamcity[setParameter name='VERSION' value='{version.short}']"""

    description = "- pps:  finalize (patch) {0} and {1}".format(VERSION, REVISION)
    user_options = [
        ("build=", "b", "build number"),
        ("revision=", "r", "changeset/revision number"),
        ("target=", None, "version file under <package root> to patch [default={0}]".format(VERSION_FILE))
    ]

    def __init__(self, dist, build=None, revision=None, target=VERSION_FILE, report=True):
        Command.__init__(self, dist)

        self.build = str(build or '')
        self.revision = str(revision or '')
        self.target = str(target)
        self.report = bool(report)

    def initialize_options(self):
        pass

    def finalize_options(self):

        if self.revision is None:
            self.revision = ""

        self.build = str(self.build)
        self.revision = str(self.revision)
        self.target = os.path.basename(self.target)

    def run(self):

        if self.build:
            self.patch_version()

        final = VersionResolver.find_version(self.target)
        message = self.TC_MESSAGE.format(version=final)

        if self.report:
            log.info(message)

        return message

    def patch_version(self):

        target = VersionResolver.find_target(self.target)
        log.info("\n # [pps] Patching {0} with b={1.build} r={1.revision}...".format(target, self))

        lines = open(target).readlines()

        with open(target, "w") as patched:
            for line in lines:
                if line.startswith(self.VERSION) or line.startswith(self.REVISION):
                    line = line.replace(self.BUILD_MACRO, self.build)
                    line = line.replace(self.REV_MACRO, self.revision)

                patched.write(line)

    @staticmethod
    def find_target(versionfile=VERSION_FILE, project=PROJECT):
        """
        Locate the version.py file of the current project.

        @param versionfile: version file name to search for (version.py by default)
        @type  versionfile: str
        @param project: root directory of the python project, i.e. a directory
                        with a setup.py file (current directory by default)
        @type project: str

        @rtype: str
        @raise VersionFileError: if no verison file can be found
        """
        pattern = os.path.join(project, "*", versionfile)
        matches = glob.glob(pattern)

        if len(matches) != 1:
            raise VersionFileError("{0} {1} files found".format(len(matches), versionfile))

        return matches[0]

    @staticmethod
    def find_package(versionfile=VERSION_FILE, project=PROJECT):
        """
        Load and return the package root name from the version.py file
        of the current project.

        @param versionfile: version file name to search for (version.py by default)
        @type  versionfile: str
        @param project: root directory of the python project, i.e. a directory
                        with a setup.py file (current directory by default)
        @type project: str

        @rtype: str
        @raise VersionFileError: if no verison file can be found
        """
        path = VersionResolver.find_target(versionfile, project)

        package = os.path.dirname(path)
        return os.path.basename(package)

    @staticmethod
    def find_version(versionfile=VERSION_FILE, project=PROJECT):
        """
        Locate the version.py file of the current project and load the
        version number of the package that we intend to build.

        @param versionfile: version file name to search for (version.py by default)
        @type  versionfile: str
        @param project: root directory of the python project, i.e. a directory
                        with a setup.py file (current directory by default)
        @type project: str

        @rtype: L{pps.version.Version}
        @raise VersionFileError: if no verison file can be found
        """
        package = VersionResolver.find_package(versionfile, project)
        target = VersionResolver.find_target(versionfile, project)

        module = {}
        vmarker = VersionResolver.VERSION
        rmarker = VersionResolver.REVISION

        exec(open(target).read(), module)

        if vmarker not in module:
            raise VersionFileError("{0} has no {1} marker".format(versionfile, vmarker))

        return Version.parse(module[vmarker], module.get(rmarker), package)

    @staticmethod
    def try_find_version(versionfile=VERSION_FILE, project=PROJECT):
        """
        Try to locate the version.py file of the current project and load the
        version number of the package that we intend to build. If no such verison
        file can be found, return a default dummy version.

        @param versionfile: version file name to search for (version.py by default)
        @type  versionfile: str
        @param project: root directory of the python project, i.e. a directory
                        with a setup.py file (current directory by default)
        @type project: str

        @rtype: L{pps.version.Version}
        """
        try:
            return VersionResolver.find_version(versionfile, project)

        except VersionFileError:
            return Version.parse("0.0.0")


class VersionError(Exception):
    pass


class Version(object):
    """
    Semantic package version number.

    Format::

        [package-]major.minor.micro[.build]

    @param package: root namespace name
    @type  package: str
    @param major: major version (incremented on incompatible changes)
    @type  major: int
    @param minor: minor version (incremented on compatible changes)
    @type  minor: int
    @param micro: patch version (incremented on minor bug fixes)
    @type  micro: int
    @param build: optional build number
    @type  build: int or str
    @param revision: optional VCS revision number
    @type  revision: str
    """
    def __init__(self, package, major, minor, micro, build=0, revision=None):

        try:
            self._package = str(package).strip()
            self._major = int(major)
            self._minor = int(minor)
            self._micro = int(micro)
            self._revision = str(revision or "")

        except ValueError as e:
            raise VersionError(str(e))

        try:
            self._build = int(build)
        except:
            self._build = 0

    @staticmethod
    def parse(string, revision=None, package=None):
        """
        Create a new Verison from string.

        @param string: version string, e.g. xx-1.2.3 or xx-1.2.3.4
        @type  string: str
        @param revision: revision number
        @type  revision: str
        @param package: package (namespace) name; if not specified, will be
                        extracted from the C{string} (if present)
        @type  package: str

        @rtype: L{Version}
        @raise VersionError: if C{string} cannot be parsed
        """
        parts = string.split("-")

        if len(parts) > 1:
            pkg = parts.pop(0)

            if package is None:
                package = pkg

        if package is not None:
            package = package.split(".")[0]

        v = parts[0].split('.')
        if not len(v) in (3, 4):
            raise VersionError("Invalid version string format: " + parts[0])

        major = v[0]
        minor = v[1]
        micro = v[2]
        build = 0

        if len(v) == 4:
            build = v[3]

        return Version(package, major, minor, micro, build, revision)

    @staticmethod
    def from_filename(filename):
        """
        Parse a package file name.

        @param filename: python package file name, e.g. package-name-1.2.3.zip
        @type  filename: str

        @rtype: L{Version}
        @raise VersionError: if C{filename} cannot be parsed
        """
        filename = os.path.splitext(os.path.basename(filename))[0]
        vstart = re.search("(\d+\.\d+.\d+)", filename)

        if not vstart:
            raise VersionError("File name {0} not in name-version-*.ext format".format(filename))

        package = filename[:vstart.start()].rstrip("-")
        filename = filename[len(package):].lstrip("-")
        short_verison = filename.split("-")[0]

        return Version.parse(short_verison, package=package)

    @property
    def package(self):
        """
        Root namespace name
        """
        return self._package

    @property
    def major(self):
        """
        Major version (incremented on incompatible changes)
        """
        return self._major

    @property
    def minor(self):
        """
        Minor version (incremented on compatible changes)
        """
        return self._minor

    @property
    def micro(self):
        """
        Patch version (incremented on minor bug fixes)
        """
        return self._micro

    @property
    def build(self):
        """
        Build number
        """
        try:
            return int(self._build)
        except:
            return self._build

    @property
    def revision(self):
        """
        VCS revision number
        """
        return self._revision

    @property
    def short(self):
        """
        Canonical three-part version number
        """
        return '{0.major}.{0.minor}.{0.micro}'.format(self)

    @property
    def full(self):
        """
        Full version, including build number
        """
        return '{0.major}.{0.minor}.{0.micro}.{0.build}'.format(self)

    def __str__(self):
        return '{0.package}-{0.short}'.format(self)

    def __repr__(self):
        return '{0.package}-{0.full}'.format(self)

    def __iter__(self):
        return iter([self.major, self.minor, self.micro, self.build])

