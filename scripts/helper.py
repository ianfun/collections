#!/usr/bin/env python
# python's interactive help and license function in C.
# run it to install
def write_help():
    from urllib.request import urlopen
    with open('license.txt', 'wb') as w:
        w.write(urlopen('https://raw.githubusercontent.com/python/cpython/main/LICENSE').read())

    from pydoc_data import topics
    from time import ctime, time
    from pydoc import Helper
    import sys
    once = 1
    with open('builtins.c', 'w', encoding='utf-8') as w:
            w.write("/*%s*/" % str(ctime(time())))
            w.write(r'''
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <assert.h>
#include <ctype.h>
static bool _continue=true;
void bui_credits(){
    puts("    Thanks to CWI, CNRI, BeOpen.com, Zope Corporation and a cast of thousands\n    for supporting Python development.  See www.python.org for more information.");
}
void bui_license(){
    FILE *li = fopen("license.txt", "r");
    if (li==NULL){
        puts("see https://www.python.org/psf/license/");
        return;
    }
    int lineno=0;
    for(int c;(c=fgetc(li))!=EOF;){
        if (c=='\n'){
            lineno += 1;
            if (lineno==23){
                fputs("\nHit Return for more, or q (and Return) to quit: ", stdout);
                if (getchar()=='q'){
                    fclose(li);
                    return;
                }
                lineno=0;
            }
        }
        putchar(c);
    }
}
void bui__Printer(char *doc){
    char *i=doc;
    for(;_continue;i+=1){
        while(*i!='\n'){
            if (*i=='\0'){
                return;
            }
            putchar(*i++);
        }
        if (getchar()==EOF)
            return;
    }
}
''')
            w.write('void bui_help_topics(char* s){\n')
            for x, y in topics.topics.items():
                    if once:
                            w.write('if (strcmp(s, "%s")==0)\n\tbui__Printer("' % (x))
                            once = 0
                    else:
                            w.write('\nelse if (strcmp(s, "%s")==0)\n\tbui__Printer("' % (x))
                    w.write(repr(y).replace('\\', '\\\\').replace('\'', '"').replace('"', '\\"').replace(r'\\n', r'\n'))
                    w.write('");')
            w.write('\nelse\n    printf("no documentation found for %s\\n", s);\n')
            w.write('}\n')
            w.write('\nvoid bui_help_interact(void){')
            w.write('\nstruct{\n\tchar *keyword;\n\tint len;\n\tchar **ths;}\nkeyword[]=\n{')
            for x, y in Helper.keywords.items():
                    w.write('{"%s", %d, (char*[])%s},\n' % (x, len(y) if type(y)==tuple else 1, ('{"%s"}' % '","'.join(y) ) if type(y)==tuple else '{"%s"}' % y))
            w.write('},\ntopics[]=\n{')
            for x, y in Helper.topics.items():
                    w.write('{"%s", %d, (char*[])%s},\n' % (x, len(y) if type(y)==tuple else 1, ('{"%s"}' % '","'.join(y) ) if type(y)==tuple else '{"%s"}' % y))
            w.write('};\n/*char* _strprefixes[]={')
            w.write(r'''"b'", "b\"", "f'", "f\"", "r'", "r\"", "u'", "u\""};*/''')
            w.write('''
    struct
    {
        char* keys[%d];
        char **values[%d];
        unsigned int len[%d];
    }_symbols_inverse=''' % (len(Helper._symbols_inverse.keys()), len(Helper._symbols_inverse.values()), len(Helper._symbols_inverse.keys())))
            w.write('''{.keys={"%s"}, .values={''' % ('","'.join(Helper._symbols_inverse.keys()).replace('\'', '"')))
            w.write(r'''
    (char* []){"\"","\"\"\"","\"","\"\"\"","b\"","b\"","f\"","f'","r\"","r\"","u\"","u\""},
    (char* []){"+","-","*","**","/","//","%","<<",">>","&","|","^","~","<",">","<=",">=","==","!=","<>"},
    (char* []){"<",">","<=",">=","==","!=","<>"},
    (char* []){"-","~"},
    (char* []){"+=","-=","*=","/=","%=","&=","|=","^=","<<=",">>=","**=","//="},
    (char* []){"<<",">>","&","|","^","~"},
    (char* []){"j","J"},
    }''')
            w.write(', .len={%s}};\n' % ','.join(str(len(x)) for x in Helper._symbols_inverse.values()))
            w.write('''struct {
        char* operator[%d];
        char* des[%d];
    }symbols = {''' % (len(Helper.symbols.keys()), len(Helper.symbols.values())))
            w.write(r'''{"%", "**", ",", ".", "...", ":", "@", "\\\\", "_", "__", "`", "(", ")", "{", "}", "\"", "\"\"\"", "\"", "\"\"\"", "b\"", "b\"", "f\"", "f\"", "r\"", "r\"", "u\"", "u\"", "+", "-", "*", "/", "//", "<<", ">>", "&", "|", "^", "~", "<", ">", "<=", ">=", "==", "!=", "<>", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>=", "**=", "//=", "j", "J"}''')
            w.write(',{"%s"}};' % '","'.join(Helper.symbols.values()))
            w.write(r"""
    void bui_leaving(int sig){
        puts("\nYou are now leaving help and returning to the Python interpreter.\nIf you want to ask for help on a particular object directly from the\ninterpreter, you can type \"help(object)\".  Executing \"help('string')\"\nhas the same effect as typing a particular string at the help> prompt.");
        _continue = false;
    }
    
    void bui_intro(){
        puts("Welcome to Python """)
            w.write(str(sys.version_info[:2]))
            w.write(r"""'s help utility!\n\nIf this is your first time using Python, you should definitely check out\nthe tutorial on the Internet at https://docs.python.org/""")
            w.write(str(sys.version_info[0]))
            w.write(
    r"""/tutorial/.\n\nEnter the name of any module, keyword, or topic to get help on writing\nPython programs and using Python modules.  To quit this help utility and\nreturn to the interpreter, just type \"quit\".\n\nTo get a list of available modules, keywords, symbols, or topics, type\n\"modules\", \"keywords\", \"symbols\", or \"topics\".  Each module also comes\nwith a one-line summary of what it does; to list the modules whose name\nor summary contain a given string such as \"spam\", type \"modules spam\".\n");
    }
    signal(SIGINT, bui_leaving);
    char buf[50]="help\n";
    bui_intro();
    do{
        fputs("\nhelp> ", stdout);
        if (fgets(buf, 50, stdin)==NULL || strcmp(buf, "q\n")==0 || strcmp(buf, "quit\n")==0){
            bui_leaving(SIGINT);
        }
        *strchr(buf, '\n')='\0';
        putchar('\n');
        if (buf[0]=='\0'||_continue==false){
            raise(SIGINT);
        }
        if (strcmp(buf, "help")==0)
            bui_intro();
        else if (strcmp(buf, "keywords")==0){
            for(size_t i=0;i<sizeof(keyword)/sizeof(keyword[0]);++i){
                fputs(keyword[i].keyword, stdout);
                putchar('\t');
            }
        }else if (strcmp(buf, "symbols")==0){
            for(unsigned int i=0;i<59;++i){
                fputs(symbols.operator[i], stdout);
                putchar('\t');
            }
        }else if (strcmp(buf, "modules")==0){
            puts("Here is a list of modules whose name or summary contains ''.\nIf there are any, enter a module name to get more help.\nPlease wait a moment while I gather a list of all available modules...");
    """)
            w.write('    puts("%s");' % '\\t'.join(sys.modules.keys()))
            w.write(r"""
            puts("Enter any module name to get more help.  Or, type \"modules spam\" to search\nfor modules whose name or summary contain the string \"spam\".");
        }else{
            char *__doc, **__relate, *_label;
            int __relatel;
            for(unsigned int i=0;i<7;++i){
                for(unsigned int j=0;j<_symbols_inverse.len[i];++j){
                    if (strcmp(buf, _symbols_inverse.values[i][j])==0){
                        for(size_t ii=0;i<sizeof(topics)/sizeof(topics[0]);++ii){
                            if (strcmp(_symbols_inverse.keys[i], topics[ii].keyword)==0){
                                for(size_t jj=0;jj<sizeof(topics)/sizeof(topics[0]);++jj){
                                    if (strcmp(topics[jj].keyword, topics[ii].ths[0])==0){
                                        __doc = topics[jj].ths[0];
                                        __relate = topics[jj].ths+1;
                                        __relatel = topics[jj].len-1;
                                        goto M;
                                    }
                                }
                                __doc = topics[ii].ths[0];
                                __relate = topics[ii].ths+1;
                                __relatel = topics[ii].len-1;
                                goto M;
                            }
                        }
                        assert(0);
                    }
                }
            }
            for(size_t i=0;i<sizeof(keyword)/sizeof(keyword[0]);++i){
                if (strcmp(buf, keyword[i].keyword)==0){
                    for(size_t j=0;j<sizeof(topics)/sizeof(topics[0]);++j){
                        if (strcmp(topics[j].keyword, keyword[i].ths[0])==0){
                            __doc = topics[j].ths[0];
                            __relate = topics[j].ths+1;
                            __relatel = topics[j].len-1;
                            goto M;
                        }
                    }
                    __doc = keyword[i].ths[0];
                    __relate = keyword[i].ths+1;
                    __relatel = keyword[i].len-1;
                }
            }
            bui_help_topics(buf);
            continue;
            M:
            _label = strdup(__doc);
            for(char *i=_label;*i;++i){
                *i=tolower(*i);
            }
            bui_help_topics(_label);
            fputs("Related help topics:", stdout);
            for (int i = 0; i < __relatel; ++i)
            {
                fputs(__relate[i], stdout);
            }
        }
        }while(_continue);
    }
    """)

write_help()