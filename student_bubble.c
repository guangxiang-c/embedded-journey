#include <stdio.h>
#include <string.h>

struct Student
{
    char name[20];
    int score;
};

int main()
{
    struct Student students[5] = {{"xiaoming", 55}, {"xiaohong", 88}, {"xiaozhang", 73}, {"xiaoli", 24}, {"xiaowang", 99}};

    for (int i = 0; i < 5 - 1; i++)
    {
        for (int j = 0; j < 5 - 1 - i; j++)
        {
            if (students[j].score < students[j + 1].score)
            {
                struct Student temp = students[j];
                students[j] = students[j + 1];
                students[j + 1] = temp;
            }
        }
    }

    // 输出分数从高到低排序结果
    for (int i = 0; i < 5; i++)
    {
        printf("%s: %d\n", students[i].name, students[i].score);
    }

    printf("==========================================\n");

    struct Student students2[5] = {{"xiaoming", 55}, {"xiaohong", 88}, {"xiaozhang", 73}, {"xiaoli", 24}, {"xiaowang", 99}};

    for (int i = 0; i < 5 - 1; i++)
    {
        for (int j = 0; j < 5 - 1 - i; j++)
        {
            if (students2[j].score > students2[j + 1].score)
            {
                struct Student temp = students2[j];
                students2[j] = students2[j + 1];
                students2[j + 1] = temp;
            }
        }
    }

    // 输出分数从低到高排序结果
    for (int i = 0; i < 5; i++)
    {
        printf("%s: %d\n", students2[i].name, students2[i].score);
    }

    printf("==========================================\n");

    struct Student students3[5] = {{"xiaoming", 55}, {"xiaohong", 88}, {"xiaozhang", 73}, {"xiaoli", 24}, {"xiaowang", 99}};

    for (int i = 0; i < 5 - 1; i++)
    {
        for (int j = 0; j < 5 - 1 - i; j++)
        {
            if (strlen(students3[j].name) > strlen(students3[j + 1].name))
            {
                struct Student temp = students3[j];
                students3[j] = students3[j + 1];
                students3[j + 1] = temp;
            }
        }
    }

    // 输出按名字长度排序（短的在前）
    for (int i = 0; i < 5; i++)
    {
        printf("%s: %d\n", students3[i].name, students3[i].score);
    }

    return 0;
}
