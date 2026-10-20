#include <stdio.h>

typedef union
{
    int i;
    char ch[sizeof(int)];
} CHI;

union AnElt
{
    int i;
    char c;
} elt1, elt2;

int main()
{
    CHI chi;
    int i;
    chi.i = 1234;
    for (i = 0; i < sizeof(int); i++)
    {
        printf("%02hhX", chi.ch[i]);
    }
    printf("\n");

    elt1.i = 4;
    elt2.c = 'a';
    printf("elt1.i=%d elt1.c=%c\n", elt1.i, elt1.c);
    printf("elt2.i=%d elt2.c=%c\n", elt2.i, elt2.c);

    union
    {
        int i;
        char c;
    } u;
    u.i = 1;
    printf("u.c=%c\n", u.c);
    if (u.c == 1)
    {
        printf("Little-Endian");
    }
    else
    {
        printf("Big-Endian");
    }

    return 0;
}