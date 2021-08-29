## Hello world

there are many way to show `Hello world`
### In python

##### simple
```python
print("Hello world")
```
or
```python
print("%s" % "Hello world")
```
##### use sys module
```python
from sys import stdout
stdout.write("Hello wolrd\n")
```
##### use ctypes module
```python
import ctypes
ctypes.cdll.msvcrt.puts(b"Hello world")
```
or
```python
import ctypes
for i in b"Hello world\n":
    ctypes.cdll.msvcrt.putchar(i)
```
or
```python
import ctypes
ctypes.cdll.msvcrt.printf(b"%s", b"Hello world\n")
```
##### overwrite for print
```python
def print(*values, **kwargs):
	from sys import stdout
	f = kwargs.get('file', stdout)
	f.write(kwargs.get('sep', ' ').join(map(str, values)))
	f.write(kwargs.get('end', '\n'))
	if kwargs.get('flush', False):
		f.flsuh()

print("Hello world")
```
### Hello world in C
using **stdio.h**
```C
puts("Hello world");

fputs("Hello world\n", stdout);

printf("%s", "Hello world");

fprintf(stdout, "%s", "Hello world");

fwrite("Hello world\n", 1, 12, stdout);

char *p="Hello world\n";
while (*p)
    putchar(*p++);

```
using **unistd.h**
```C
write(STDIN_FILENO, "Hello world", 12);
```