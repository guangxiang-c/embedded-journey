#include <stdio.h>

int main()
{
    int scores[10];
    printf("please enter ten scores(0~100):");
    

    for (int i = 0; i < 10; i++) {
        scanf("%d", &scores[i]);
    }

    int max = scores[0];
    int min = scores[0];
    int sum = 0;
    double average;
    int passCounts = 0;

    for (int i = 0; i < 10; i++) {
        sum += scores[i];
        if(scores[i] > max) {
            max = scores[i];
        }
        if(scores[i] < min) {
            min = scores[i];
        }
    }

    average = sum / 10.0;

    for (int i = 0; i < 10; i++) {
        if(scores[i] >= 60) {
            passCounts++;
        }
    }

    printf("max=%d,min=%d,average score=%.1f,pass %d person\n", max, min, average, passCounts);
}