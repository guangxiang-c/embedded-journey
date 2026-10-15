#include <stdio.h>

int mystrlen(const char *s) {
    int cnt = 0;
    while(s[cnt] != '\0') {
        cnt ++;
    }
    return cnt;
}

int main()
{
    // char x[] = "hello world !";
    char x[100] = "";
    printf("please enter your word:");
    scanf("%s", x);
    printf("your word length=%d", mystrlen(x));

    return 0;

}

