import os

let params = commandLineParams()

if params.len < 1:
  quit "Usage: [file.bf]"

proc toAsm(f: File, bf: string) =
  for c in bf:
    f.write(case c:
    of '+': "++*ptr;"
    of '-': "--*ptr;"
    of '>': "++ptr;"
    of '<': "--ptr;"
    of '.': "putchar((int)*ptr);"
    of ',': "*ptr=getchar();"
    of '[': "while(*ptr){"
    of ']': "}"
    else:
      "")
var f = open("out.c", fmWrite)
f.write("""
#include <stdio.h>
int main(void){char TAPE[8000]={0},*ptr=TAPE;
""")
try:
  toAsm(f, readFile(params[0]))
except IndexError:
  echo "error: syntax error"
f.write("return 0;}")
close(f)
echo "done!"
