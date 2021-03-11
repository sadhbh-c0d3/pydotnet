import sys
import tarfile


try:
    from urllib import urlretrieve
except:
    from urllib.request import urlretrieve

url = sys.argv[1]

print("Downloading " + url)
filename, headers = urlretrieve(url)

print("Extracting archive " + filename)
tarfile.open(filename).extractall()
