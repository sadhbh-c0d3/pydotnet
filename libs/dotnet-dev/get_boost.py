import os
import sys
import tarfile

try:
    from urllib import urlretrieve
except:
    from urllib.request import urlretrieve

def get_download_url(version):
    filename = 'boost_' + version.replace('.', '_') + '.tar.gz'
    url = (
        'https://boostorg.jfrog.io/ui/api/v1/download?repoKey=main&path=release%252F'
        + version + '%252Fsource%252F' + filename
    )
    return url, filename


url, filename = get_download_url(sys.argv[1])

print("Downloading " + url)
old_filename, headers = urlretrieve(url)
os.rename(old_filename, filename)

print("Extracting archive " + filename)
tarfile.open(filename).extractall()

