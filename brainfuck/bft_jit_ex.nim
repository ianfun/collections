#  x86-64 brainfuck jit compiler
#  tested in Windows 11 x86-64
#
#  r12: tape ptr
#  r13: getchar
#  r14: putchar
#
#  switch c:
#  case '+':
#    41 fe 04 24             incb   (%r12)
#  case '-':
#    41 fe 0c 24             decb   (%r12)
#  case '>':
#    49 ff c4                inc    %r12 
#  case '<':
#    49 ff cc                dec    %r12
#  case '.':
#    41 0f b6 0c 24          movzbl  (%r12),%ecx
#    41 ff d6                callq  *%r14
#  case ',':
#    41 ff d5                callq  *%r13
#    41 88 04 24             mov    %al,(%r12)
#  case '[':
#    e9 <4 byte>             jmpq   (jump to close `]`)
#  case ']': 
#    41 8a 04 24             mov    (%r12),%al
#    84 c0                   test   %al,%al
#    0f 85 <4 byte>          jne    (if no-zero, jump to `[` next)
#

{.passC: "-Wall -Wextra".}

from os import commandLineParams, getAppFilename
import lib

var
   jump = newSeqOfCap[int32](30)
   tape: array[5000, int8]
   c: int32 = 0
   cur: int32 = 0


proc getCodeSize(bf: string): csize_t = 
  result = 38
  var b = 0
  for i in bf:
    case i:
    of '+', '-':
      result += 5
    of '>', '<':
      result += 7
    of '.':
      result += 8
    of ',':
      result += 7
    of '[':
      result += 5
      inc b
    of ']':
      result += 12
      dec b
    else:
      discard
  if b != 0:
    quit "error: syntax error: unmatched `[` or too many `]`"

let params = commandLineParams()

if params.len < 1:
  echo "Usage: ", getAppFilename(), " [file.bf]"
  quit 1

let
  bf = readFile params[0]
  code_size = ((getCodeSize bf) div 4096) * 4096 + 4096

var mem = cast[ptr UncheckedArray[uint8]](VirtualAlloc(nil, code_size, MEM_RESERVE or MEM_COMMIT, PAGE_READWRITE))


proc emit(a: openarray[uint8]) = 
  for i in a:
    mem[c] = i
    inc c

proc emit(a: int32) =
  cast[ptr UncheckedArray[int32]](addr mem[c])[0] = a
  c += 4

proc emit(a: uint8) =
  mem[c] = a
  inc c

proc jit_bf(): int32 = 
  while true:
    if cur == bf.len:
      return
    case bf[cur]:
    of '+':
      var n = 0'u8
      while bf[cur]=='+':
        inc cur
        inc n
      dec cur
      emit(openarray([0x41'u8, 0x80'u8, 0x04'u8, 0x24'u8]))                # addb   $n, (%r12)
      emit(n)
    of '-':
      var n = 0'u8
      while bf[cur]=='-':
        inc cur
        inc n
      dec cur
      emit(openarray([0x41'u8, 0x80'u8, 0x2c'u8, 0x24'u8]))               # subb   $0xa,(%rcx)
      emit(n)
    of '>': 
      var n = 0.int32
      while bf[cur]=='>':
        inc cur
        inc n
      dec cur
      emit(openarray([0x49'u8, 0x81'u8, 0xc4'u8]))                       # addq    $n, %r12
      emit(n)
    of '<': 
      var n = 0.int32
      while bf[cur]=='<':
        inc cur
        inc n
      dec cur
      emit(openarray([0x49'u8, 0x81'u8, 0xec'u8]))                       # subq    $n, %r12
      emit(n)
    of '.':
      emit(openarray([0x41'u8, 0x0f'u8, 0xb6'u8, 0x0c'u8, 0x24'u8,       # movzbl (%r12),%ecx
                      0x41'u8, 0xff'u8, 0xd6'u8]))                       # callq  *%r12 (putchar)
    of ',':
      emit(openarray([0x41'u8, 0xff'u8, 0xd5'u8,                         # callq  *%r13 (getchar)
                       0x41'u8, 0x88'u8, 0x04'u8, 0x24'u8]))             # mov    %al,(%r12)
    of '[':
      let w = c
      emit(openarray([0xe9'u8, 0'u8, 0'u8, 0'u8, 0'u8]))
      inc cur
      jump.add(c)
      let loc = c
      cast[ptr UncheckedArray[int32]](addr mem[w+1])[0] = jit_bf() - loc
    of ']':
      let here = c
      emit(openarray([0x41'u8, 0x8a'u8, 0x04'u8, 0x24'u8, # mov    (%r12),%al
                      0x84'u8, 0xc0'u8,                   # test   %al,%al
                      0x0f'u8, 0x85'u8]                   # jne    <4 byte address>
       ))
      emit(jump.pop() - (c + 4))
      return here
    else:
      discard
    inc cur

proc jit_compile() = 
  emit(openarray([0x55'u8,                            # push   %rbp
                  0x41'u8, 0x54'u8,                   # push   %r12
                  0x41'u8, 0x55'u8,                   # push   %r13
                  0x41'u8, 0x56'u8,                   # push   %r14
                  0x48'u8, 0x89'u8, 0xe5'u8,          # mov    %rsp,%rbp
                  0x48'u8, 0x83'u8, 0xec'u8, 0x20'u8, # sub    $0x20,%rsp
                  0x48'u8, 0x89'u8, 0xe5'u8,          # mov    %rsp,%rbp
                  0x49'u8, 0x89'u8, 0xcc'u8,          # mov    %rcx,%r12 (tape)
                  0x49'u8, 0x89'u8, 0xd5'u8,          # mov    %rdx,%r13 (getchar)
                  0x4d'u8, 0x89'u8, 0xc6'u8]))        # mov    %r8,%r14 (putchar)
  discard jit_bf()
  emit(openarray([
                  0x48'u8, 0x83'u8, 0xc4'u8, 0x20'u8, # add    $0x20,%rsp
                  0x41'u8, 0x5e'u8,                   # pop    %r14
                  0x41'u8, 0x5d'u8,                   # pop    %r13
                  0x41'u8, 0x5c'u8,                   # pop    %r12
                  0x5d'u8,                            # pop    %rbp
                  0xc3'u8]))                          # retq

if mem==nil:
  quit "VirtualAlloc failed"

jit_compile()

var ig: DWORD

if VirtualProtect(mem, code_size, PAGE_EXECUTE_READ, addr ig)!=1:
  VirtualFree(mem, code_size, 0)
  quit "VirtualProtect failed"

proc jitfun(tape: typeof(tape), getc: typeof(getchar), putc: typeof(putchar)): void = discard

(cast[typeof(jitfun)](mem))(tape, getchar, putchar)


VirtualFree(mem, code_size, 0)
