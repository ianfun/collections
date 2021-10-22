#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
static unsigned char *code = (unsigned char*)"+[.,]";
//static unsigned char* code = (unsigned char*)"++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.";
static unsigned char *ptr=NULL;
static void interpreter(){
	for(;;){
		switch(*code){
			case '\0': puts("--exit--");return;
			case '+' : ++*ptr;break;
			case '-' : --*ptr;break;
			case '.' : putchar(*ptr);break;
			case ',' : *ptr=getchar();break;
			case '>' : ++ptr;break;
			case '<' : --ptr;break;
			case '[' :
					   if(*ptr==0){
					   	    do
					   	    {
					   	        code+=1;
  					   	    } while (*code!=']');
					   }else{
							unsigned char* temp=code+1;
					   		do {
					   			code=temp;
					   			interpreter();
					   		}while (*ptr);
					   	}
					   	break;
			case ']' : return;
			default  : printf("? %c(%d) ?\n", *code, (int)*code);
		}
		code+=1;
	}
}

int main(void){
	puts("--running--");
#ifdef __cplusplus
	ptr = (unsigned char*)calloc(1, 1000);
#else
	ptr = calloc(1, 1000);
#endif
	assert(ptr!=NULL);
	interpreter();
	return 0;
}
