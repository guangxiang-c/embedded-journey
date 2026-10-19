#include <stdio.h>

void bubble_sort(int arr[], int len);

int main()
{
    int arr[6] = {9, 3, 7, 1, 8, 2};

    printf("before:");
    for (int i = 0; i < 6; i++)
    {
        printf("%d ", arr[i]);
    }

    bubble_sort(arr, 6);

    printf("\n");
    printf("after:");
    for (int i = 0; i < 6; i++)
    {
        printf("%d ", arr[i]);
    }

    return 0;
}

void bubble_sort(int arr[], int len)
{
    for (int i = 0; i < len - 1; i++)
    {
        for (int j = 0; j < len - i - 1; j++)
        {
            int temp;
            if (arr[j] > arr[j + 1])
            {
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}