/* Вправа 2. Шифр Цезаря та обробка слів (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
void caesar(char *dst, const char *src, int shift) {
    shift=((shift%26)+26)%26;
    for(int i=0;src[i];i++){
        char c=src[i];
        if(isupper(c))      dst[i]='A'+(c-'A'+shift)%26;
        else if(islower(c)) dst[i]='a'+(c-'a'+shift)%26;
        else                dst[i]=c;
    }
    dst[strlen(src)]='\0';
}
int count_words(const char *s){
    int count=0,in_word=0;
    for(int i=0;s[i];i++){
        if(!isspace(s[i])){if(!in_word){count++;in_word=1;}}
        else in_word=0;
    }
    return count;
}
void reverse_words(char *s){
    /* Перевернути весь рядок */
    int n=strlen(s),l=0,r=n-1;
    while(l<r){char t=s[l];s[l++]=s[r];s[r--]=t;}
    /* Перевернути кожне слово */
    l=0;
    for(int i=0;i<=n;i++){
        if(i==n||isspace(s[i])){
            int ll=l,rr=i-1;
            while(ll<rr){char t=s[ll];s[ll++]=s[rr];s[rr--]=t;}
            l=i+1;
        }
    }
}
int main(void) {
    console_utf8();
    char plain[256],enc[256],dec[256];
    int shift;
    printf("Рядок: "); fgets(plain,sizeof(plain),stdin);
    plain[strcspn(plain,"\n")]='\0';
    printf("Зсув: "); scanf("%d",&shift);
    caesar(enc,plain,shift);
    caesar(dec,enc,-shift);
    printf("Оригінал:    %s\n",plain);
    printf("Зашифровано: %s\n",enc);
    printf("Розшифровано:%s\n",dec);
    printf("Слів у рядку: %d\n",count_words(plain));
    char buf[256]; strcpy(buf,plain);
    reverse_words(buf);
    printf("Слова навпаки: %s\n",buf);
    return 0;
}
