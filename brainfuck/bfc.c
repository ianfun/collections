#include <stdio.h>
#include <windows.h>
#include <stdint.h>

static
   uint8_t TYPE[1024], *ptr=TYPE;

void brainfuck(char* code){
	for(;;code+=1){
		switch (*code){
			case '>': ++ptr;
					  break;
			case '<': --ptr;
					  break;
			case '+': ++*ptr;
					  break;
			case '-': --*ptr;
					  break;
			case '.': putchar(*ptr);
					  break;
			case ',': *ptr = getchar();
					  break;
			case '[':
					{
				  		while (*ptr){
				  			brainfuck(code+1);
				  		}
				  		int b = 0;
				  		do {
				  			code += 1;
				  			switch (*code){
				  				case '[': ++b;break;
				  				case ']': --b;break;
				  			}
				  		} while (b >= 0);
					}
					break;
			case ']': return;
			case '\x0': return;
		}
	}
}

int main(int argc, char **argv)
{
	if (argc < 2)
		fprintf(stderr, "Usage: [%s] [file.bf]\n", *argv);
	else {
		FILE *f = fopen(argv[1], "rb");
		if (f==NULL)
			perror(argv[1]);
		else {
			fseek(f, 0, SEEK_END);
			size_t size = (size_t)ftell(f);
			fseek(f, 0, SEEK_SET);
			char *code = malloc(size+1);
			code[size] = 0;
			if (ptr==NULL)
				perror("malloc");
			else {
				if (fread(code,  size, 1, f)<0)
					perror("fread");
				else
					brainfuck(code);
			}
			fclose(f);
		}
	}
}