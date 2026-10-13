#include <stdio.h>
#include <stdlib.h>
#include <time.h>



int main()
{
    srand(time(0));
    int answer = rand() % 100 + 1;

    int guessNum;
    int count = 0;
    
    while (1) {
        printf("please guess number:");
        scanf("%d", &guessNum); 
        count++;
        if (guessNum > answer) {
            printf("Too high ");
        } else if (guessNum < answer){
            printf("Too low ");
        } else {
            printf("Congratulations! The answer is %d, you guessed %d times", answer, count);
            break;
        }
    };
}