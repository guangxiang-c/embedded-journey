#include <stdio.h>
#include <string.h>

struct Student
{
    char name[20];
    int score;
};

int notNeed(struct Student sortStudents[], char name[20]);

int main()
{
    struct Student students[5] = {{"xiaoming", 55}, {"xiaohong", 88}, {"xiaozhang", 73}, {"xiaoli", 24}, {"xiaowang", 99}};
    struct Student sortStudents[5];
    struct Student maxScoreStudent;

    for (int i = 0; i < 5; i++)
    {
        maxScoreStudent.score = -1;
        for (int j = 0; j < 5; j++)
        {
            if (students[j].score > maxScoreStudent.score && notNeed(sortStudents, students[j].name))
            {
                maxScoreStudent = students[j];
            }
        }
        sortStudents[i] = maxScoreStudent;
    }

    // 输出排序结果
    for (int i = 0; i < 5; i++)
    {
        printf("%s: %d\n", sortStudents[i].name, sortStudents[i].score);
    }

    return 0;
}

int notNeed(struct Student sortStudents[], char name[20])
{
    int isNeed = 1;

    for (int i = 0; i < 5; i++)
    {
        if (strcmp(sortStudents[i].name, name) == 0)
        {
            isNeed = 0;
        }
    }

    return isNeed;
}
