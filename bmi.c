#include <stdio.h>

int main()
{
    double height = 0.0;
    double weight = 0.0;
    double bmi = 0.0;

    printf("input height(m) and weight(kg)");
    scanf("%lf %lf", &height, &weight);

    bmi = weight/height/height;
    printf("BMI: %.2f", bmi);

    if (bmi> 0.0 && bmi <18.5) {
        printf(" Underweight");
    } else if (bmi >=18.5 && bmi<24.0) {
        printf(" Normal");
    } else if (bmi>=24.0) {
        printf(" Overweight");
    }

    return 0;
}