#include <stdio.h>
#include "lab01a.h"

int main()
{
    int i = j;      /* навмисна помилка: j ще не оголошено */
    int j = i + 10;
    printf("%d %d\n", i, j);
    return 0;
}
