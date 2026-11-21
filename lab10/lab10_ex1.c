/* Вправа 1. Статистика рядка (з методичних вказівок) */
#include "../common_console.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
int main(void) {
    console_utf8();
    char s[256];
    printf("Введіть рядок: ");
    fgets(s, sizeof(s), stdin);
    s[strcspn(s,"\n")] = '\0';
    int letters=0,digits=0,spaces=0,upper=0,lower=0,other=0;
    int freq[256]={0};
    for (int i=0; s[i]; i++) {
        unsigned char c=s[i];
        freq[c]++;
        if      (isupper(c)){upper++;letters++;}
        else if (islower(c)){lower++;letters++;}
        else if (isdigit(c)) digits++;
        else if (isspace(c)) spaces++;
        else                 other++;
    }
    printf("Довжина: %zu\n",strlen(s));
    printf("Літери: %d (вел: %d, мал: %d)\n",letters,upper,lower);
    printf("Цифри: %d, Пробіли: %d, Інші: %d\n",digits,spaces,other);
    /* Паліндром? */
    int n=strlen(s),ok=1;
    for(int i=0;i<n/2;i++)
        if(tolower(s[i])!=tolower(s[n-1-i])){ok=0;break;}
    printf("Паліндром: %s\n",ok?"так":"ні");
    /* Символи з частотою >= 2 */
    printf("Частотні символи (>=2):\n");
    for(int c=32;c<127;c++)
        if(freq[c]>=2) printf("  '%c': %d\n",c,freq[c]);
    return 0;
}
