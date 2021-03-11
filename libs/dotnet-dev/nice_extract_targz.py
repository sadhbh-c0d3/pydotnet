from __future__ import print_function
import sys
import os
import tarfile


class ExtractTarGzTracker(object):
    def __init__(self, exclude_paths = []):
        self.__longest_status = 0
        self.__file_number = 0
        self.__exclude_paths = exclude_paths

    def __call__(self, member):
        self.__file_number += 1
        status = r"%10d %s" % (self.__file_number, member.path)
        if any(x in member.path for x in self.__exclude_paths):
            return False
        if len(status) > self.__longest_status:
            self.__longest_status = len(status)
        status = status.ljust(self.__longest_status)
        status = status + chr(8)*(len(status)+1)
        print(status, end='')
        return True


def extract_targz(path, tracker):
    print("Extracting: %s" % (path,))
    def track_progress(members):
        for member in members:
            if tracker(member):
                yield member
    with tarfile.open(path, 'r') as tarball:
        tarball.extractall(members = track_progress(tarball))


if __name__ == '__main__':
    extract_targz(sys.argv[1], ExtractTarGzTracker(sys.argv[2:]))

