#include <stdio.h>

int main()
{
    int arr[8] = {23, 11, 45, 2, 67, 34, 89, 5};

    // for (int i = 0; i < 8; i++)
    // {
    //     for (int j = i; j < 8 - i; j++)
    //     {
    //         if (arr[j] < arr[j + 1])
    //         {
    //             int temp = arr[j];
    //             arr[j] = arr[j + 1];
    //             arr[j + 1] = temp;
    //         }
    //     }
    // }

    // 参考老代码
    for (int i = 0; i < 8 - 1; i++)
    {
        for (int j = 0; j < 8 - 1 - i; j++)
        {
            if (arr[j] > arr[j + 1])
            {
                int temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < 8; i++)
    {
        printf("%d ", arr[i]);
    }
}