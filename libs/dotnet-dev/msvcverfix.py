import os
import sys
import glob

def fix_file_msvc_version(path):
   os.rename(path, path.replace('vc150','vc141').replace('vc160','vc142'))


def fix_folder_msvc_version(dirpath):
   for path in glob.glob(dirpath):
      fix_file_msvc_version(path)


if __name__ == '__main__':
   fix_folder_msvc_version(sys.argv[1] + '/*.dll')
   fix_folder_msvc_version(sys.argv[1] + '/*.lib')

