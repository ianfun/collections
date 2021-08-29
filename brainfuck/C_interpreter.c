#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

unsigned char *code = (unsigned char*)"++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>.";


unsigned char *ptr;
void interpreter(void){
	while(1){
		switch(*code){
			case '\0': return;
			case '+' : ++*ptr;break;
			case '-' : --*ptr;break;
			case '.' : putchar(*ptr);break;
			case ',' : *ptr=getchar();break;
			case '>' : ++ptr;break;
			case '<' : --ptr;break;
			case '[' : {unsigned char* temp=++code;while(*ptr){code=temp;interpreter();}}while(*code!=']'){code+=1;};break;
			case ']' : return;
			default  : break;
		}
		++code;
	}
}

int main(void){
#ifdef __cplusplus
	ptr = (unsigned char*)calloc(1, 200);
#else
	ptr = calloc(1, 200);
#endif
	interpreter();
	return 0;
}
