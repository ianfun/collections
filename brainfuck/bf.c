#include <stdio.h>
#include <windows.h>
#include <stdint.h>

static uint8_t TYPE_DATA[500] = {0};
static uint8_t* ptr = TYPE_DATA;
static uint8_t* jmp_back[100] = {0};
static uint8_t** back = jmp_back;

void brainfuck(uint8_t* code){
	for(;;){
		switch (*code){
			case '>': ptr += 1;
					  break;
			case '<': ptr -= 1;
					  break;
			case '+': *ptr += 1;
					  break;
			case '-': *ptr -= 1;
					  break;
			case '.': putchar(*ptr);
					  break;
			case ',': *ptr = getchar();
					  break;
			case '[':
						*++back = code; // note: since `code+1` after `code=*back`, code points to the next char after `[`
						while (*++code!=']')
							;
			case ']':
						if (*ptr) {code = *back;}
						else {back -= 1;}
						break;
			case '\0': return;
			default:   printf("unexpected '%c' (%d)\n", *code, (int)*code);
		}
		code += 1;
	}
}

int main(void)
{
	brainfuck((uint8_t*)"++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.");
}