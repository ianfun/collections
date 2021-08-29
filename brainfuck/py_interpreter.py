import sys

if len(sys.argv)<2:
    print("Usege: python py_i.py [-Brainfuck file]")
    sys.exit(0)

TYPE = bytearray(200)
ptr = 0
temp = 0

with open(sys.argv[1], 'rb') as f:
    code=f.read() + b'\0'
code_i = 0

def interpreter():
    global code_i, ptr
    while 1:
        if code[code_i]==ord('\0') or code[code_i]==ord(']'):
            return
        if code[code_i]==ord('+'):
            TYPE[ptr]=TYPE[ptr]+1
        elif code[code_i]==ord('-'):
            TYPE[ptr]=TYPE[ptr]-1
        elif code[code_i]==ord('<'):
            ptr-=1
        elif code[code_i]==ord('>'):
            ptr+=1
        elif code[code_i]==ord(','):
            TYPE[ptr]=sys.stdin.read(1).encode('utf-8')[0]
        elif code[code_i]==ord('.'):
            sys.stdout.write(chr(TYPE[ptr]))
        elif code[code_i]==ord('['):
            code_i+=1
            temp=code_i
            while(TYPE[ptr]):
                code_i=temp
                interpreter()
        code_i+=1

interpreter()

