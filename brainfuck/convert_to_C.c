/*
convert Brainfuck to C.
Usage: converter [hello.bf]
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
FILE *r, *w, *gcc, *exe;
int main(int argc, char **argv)
{
    if (argc!=2){
        puts("missing file");
        return 0;
    }
    assert(r=fopen(argv[1], "r"));
    fseek(r, 0, SEEK_END);
    long len = ftell(r);
    char* buf = calloc(1, len+1);
    rewind(r);
    fread(buf, 1, len, r);
    fclose(r);
    assert(w = fopen("temp.c", "w"));
    fputs("#include <assert.h>\n#include <stdio.h>\n#include <stdlib.h>\nint main(int argc, char const *argv[]){char* ptr;\nassert(ptr=calloc(1, 200));\n", w);
    while(1){
        switch(*buf++){
            case '\0':{
                fputs("}", w);
                fclose(w);
                char b[50];
                gcc=popen("gcc -Wall -o temp.exe temp.c", "r");
                while(fgets(b, 50, gcc)) fputs(b, stderr);
                assert(fclose(gcc)==0);
                assert(exe=popen("temp.exe", "r"));
                while(fgets(b, 50, exe)) fputs(b, stdout);
                fclose(r);
                return 0;
            }
            case '+':fputs("++*ptr;\n", w);break;
            case '-':fputs("--*ptr;\n", w);break;
            case '>':fputs("++ptr;\n", w);break;
            case '<':fputs("--ptr;\n", w);break;
            case '[':fputs("while(*ptr){\n", w);break;
            case ']':fputs("}\n", w);break;
            case ',':fputs("*ptr=getchar();\n", w);break;
            case '.':fputs("putchar(*ptr);\n", w);break;
            default: fputs("warning: ignore input ", stderr);fputc(buf[-1], stderr);
        }
    }
}