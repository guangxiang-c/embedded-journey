#include <stdio.h>

void reverse(int arr[], int);

int main()
{
    int arr[] = {34, 45, 3, 65, 23};
    int len = sizeof(arr)/sizeof(arr[0]);

    printf("before: ");
    for(int i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    reverse(arr, len);

    printf("after: ");
    for(int i = 0; i < len; i++) {
        printf("%d ", arr[i]);
    }

    return 0;
}

// void reverse(int arr[], int len)
// {
//     int arr2[len];

//     for(int i = 0; i < len; i++) {
//         arr2[i] = arr[i];
//     }
//     for(int i = 0; i < len; i++) {
//         arr[i] = arr2[len - 1 - i];
//     }
// }

void reverse(int arr[], int len)
{
    int *left = arr;
    int *right = arr + len -1;

    while (left < right)
    {
        int temp = *left;
        *left = *right;
        *right = temp;
        left++;
        right--;
    }
    
}
