when sizeof(pointer)!=8:
  {.error: "require x86-64 Architecture".}

type
  DWORD* {.importc: "DWORD", nodecl.} = culong
  WINBOOL* {.importc: "WINBOOL", nodecl.} = cint

proc VirtualAlloc*(lpAddress: pointer, dwSize: csize_t, flAllocationType, flProtect: DWORD): pointer {.importc: "VirtualAlloc", header: "windows.h", nodecl.}
proc VirtualProtect*(lpAddress: pointer, dwSize: csize_t, flNewProtect: DWORD, lpflOldProtect: ptr DWORD): WINBOOL {.importc: "VirtualProtect", header: "windows.h", nodecl.}
proc VirtualFree*(lpAddress: pointer, dwSize: csize_t, dwFreeType: DWORD) {.importc: "VirtualFree", header: "windows.h", nodecl.}

var
   MEM_RESERVE* {.importc: "MEM_RESERVE", nodecl.}: DWORD 
   MEM_COMMIT* {.importc: "MEM_COMMIT", nodecl.}  : DWORD
   PAGE_READWRITE* {.importc: "PAGE_READWRITE", nodecl.} : DWORD
   PAGE_EXECUTE_READ* {.importc: "PAGE_EXECUTE_READ", nodecl.}: DWORD


proc getchar*(): cint {.importc: "getchar".}
proc putchar*(c: cint): cint {.importc: "putchar".}

