# Nim is fast ...
# https://www.python.org/ fib with Nim
iterator fib(c: int): int =
    var 
        a=0
        b=1
        temp = 0
    while a<c:
        temp = a
        a = b
        b += temp
        yield a

for i in fib(1000):
    stdout.write($ i & ' ')
