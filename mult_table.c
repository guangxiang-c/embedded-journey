#include <stdio.h>

int main()
{
    for(int i=1; i<=9; i++) {
        for (int j=1; j<=i; j++) {
            if (i == j) {
                printf("%d x %d = %2d\n", j, i, i * j);
            } else {
                printf("%d x %d = %2d   ", j, i, i * j);
            }
            
        }
    }
}