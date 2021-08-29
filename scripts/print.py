import sys

def print(*args, **kwlist):
      f = kwlist.get('file', sys.stdout)
      f.write(kwlist.get('sep', ' ').join(map(str, args)))
      f.write(kwlist.get('end', '\n'))
      if kwlist.get('flush', False):
            f.flush()

