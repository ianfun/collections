#include <stdio.h>
#include <windows.h>
#include <stdint.h>

int main(){
	unsigned char code[] = {
		0xb8, 
		0, 0, 0, 0, 
		0xc3
	};
	code[1] = (int32_t)rand();

	SYSTEM_INFO info;
	GetSystemInfo(&info);
	size_t pagez = info.dwPageSize;

	uint32_t (*exe)(void) = VirtualAlloc(NULL, pagez, MEM_COMMIT, PAGE_READWRITE);

	if (exe==NULL){
		int err = GetLastError();
		printf("VirtualAlloc failed: %d\n", err);
		return err;
	}

	memcpy(exe, code, sizeof code);
	DWORD sizez;
	VirtualProtect(exe, sizeof code, PAGE_EXECUTE_READ, &sizez);

	for (int i = 0; i < 10; ++i)
	{
		printf("%d\n", exe());
	}

	VirtualFree(exe, 0, MEM_RELEASE);
}
