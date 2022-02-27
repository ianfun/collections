#include <stdio.h>
#include <windows.h>
#include <stdint.h>

static uint8_t TYPE_DATA[500] = {0};
static uint8_t* ptr = TYPE_DATA;

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
					  while (*ptr){
					  	brainfuck(code+1);
					  }
					  do {
					  	code += 1;
					  } while (*code!=']');
					  break;
			case ']': return;
			case '\x0': return; // end of string
			default:   printf("unexpected '%c' (%d)\n", *code, (int)*code);
		}
		code += 1;
	}
}

int main(void)
{
	brainfuck((uint8_t*)"++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.");
}