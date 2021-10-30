import sys
import tarfile


url = sys.argv[1]

print("Extracting archive " + filename)
tarfile.open(filename).extractall()

