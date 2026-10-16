#include <stdio.h>

struct Complex
{
    double real; // 实部
    double imag; // 虚部
};

// 复数加
struct Complex add(struct Complex a, struct Complex b)
{
    struct Complex r;
    r.real = a.real + b.real;
    r.imag = a.imag + b.imag;
    return r;
}

// 复数减
struct Complex sub(struct Complex a, struct Complex b)
{
    struct Complex r;
    r.real = a.real - b.real;
    r.imag = a.imag - b.imag;
    return r;
}

// 复数乘
struct Complex mul(struct Complex a, struct Complex b)
{
    struct Complex r;
    r.real = a.real * b.real - a.imag * b.imag;
    r.imag = a.real * b.imag + a.imag * b.real;
    return r;
}

int main()
{
    struct Complex addResult = add((struct Complex){1.0, 2.0}, (struct Complex){3.0, 4.0});
    struct Complex subResult = sub((struct Complex){1.0, 2.0}, (struct Complex){3.0, 4.0});
    struct Complex mulResult = mul((struct Complex){1.0, 2.0}, (struct Complex){3.0, 4.0});

    printf("add result=%.1f + %.1fi\n", addResult.real, addResult.imag);
    printf("sub result=%.1f + %.1fi\n", subResult.real, subResult.imag);
    printf("mul result=%.1f + %.1fi\n", mulResult.real, mulResult.imag);

    return 0;
}