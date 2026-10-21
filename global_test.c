#include <stdio.h>

int add_once(void);

int count = 0; // 对全局变量进行++操作，会累积

int main()
{
    int count = 10; // 遮蔽：局部变量覆盖全局变量

    // add_once();
    printf("first  count=%d local=%d\n", add_once(), count);

    // add_once();
    printf("second count=%d local=%d\n", add_once(), count);

    // add_once();
    printf("third  count=%d local=%d\n", add_once(), count);

    return 0;
}

int add_once(void)
{
    count++;
    printf("count=%d\n", count);
    return count;
}
