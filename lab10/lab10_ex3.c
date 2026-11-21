/* Вправа 3. Балансування дужок та власні функції (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
#include <string.h>
/* Власна strlen */
size_t my_strlen(const char *s){
    const char *p=s; while(*p) p++; return p-s;
}
/* Власна strcmp */
int my_strcmp(const char *a, const char *b){
    while(*a&&*a==*b){a++;b++;}
    return (unsigned char)*a-(unsigned char)*b;
}
/* Перевірка балансу дужок */
int check_brackets(const char *s){
    char stack[256]; int top=0;
    for(int i=0;s[i];i++){
        char c=s[i];
        if(c=='('||c=='['||c=='{') stack[top++]=c;
        else if(c==')'||c==']'||c=='}'){
            if(top==0) return 0;
            char open=stack[--top];
            if((c==')'&&open!='(')||(c==']'&&open!='[')||(c=='}'&&open!='{'))
                return 0;
        }
    }
    return top==0;
}
int main(void) {
    console_utf8();
    /* Тест власних функцій */
    const char *s="Hello World";
    printf("strlen=%zu  my_strlen=%zu\n",strlen(s),my_strlen(s));
    printf("strcmp: %d  my_strcmp: %d\n",strcmp("abc","abd"),my_strcmp("abc","abd"));
    /* Тест балансу дужок */
    const char *tests[]={
        "({[]})","([)]","{[()()]}","(((",""};
    for(int i=0;tests[i][0]||i==4;i++){
        if(i==4) break;
        printf("\"%s\": %s\n",tests[i],
               check_brackets(tests[i])?"збалансовано":"незбалансовано");
    }
    return 0;
}
