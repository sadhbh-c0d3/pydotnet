from __future__ import print_function
import sys
try:
    from urllib2 import urlopen
    def get_contentlength(u):
        return int(u.info().getheaders("Content-Length")[0])
except:
    from urllib.request import urlopen
    def get_contentlength(u):
        return int(u.info()['Content-Length'])


def download(url, file_name):
    print("Openning: %s" % (url,))
    u = urlopen(url)
    with open(file_name, 'wb') as f:
        meta = u.info()
        file_size = get_contentlength(u)
        print("Downloading: %s Bytes: %s" % (file_name, file_size))
        file_size_dl = 0
        block_sz = 64*1024
        while True:
            buffer = u.read(block_sz)
            if not buffer:
                break
            file_size_dl += len(buffer)
            f.write(buffer)
            status = r"%10d  [%3.2f%%]" % (file_size_dl, file_size_dl * 100. / file_size)
            status = status + chr(8)*(len(status)+1)
            print(status, end="")
        f.close()


if __name__ == '__main__':
    download(sys.argv[1], sys.argv[2])

