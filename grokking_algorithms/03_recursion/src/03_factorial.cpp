//
// Created by wuxianggujun on 2025/3/9.
//

#include <iostream>

using std::cout;
using std::endl;

/**
 * 阶乘函数的递归实现
 * 阶乘定义: n! = n * (n-1) * (n-2) * ... * 2 * 1
 * 例如: 5! = 5 * 4 * 3 * 2 * 1 = 120
 * 
 * @param x 要计算阶乘的非负整数
 * @return x的阶乘结果
 */
int fact(const int& x)
{
    // 基本情况(Base case): 当x为1时，1的阶乘就是1，递归在此终止
    if (x==1) return 1;
    
    // 递归情况(Recursive case): x的阶乘等于(x-1)的阶乘乘以x
    // 递归调用fact(x-1)计算(x-1)的阶乘，然后乘以x
    return fact(x-1) * x;
}

int main()
{
    // 计算并输出5的阶乘
    // 递归过程: fact(5) = fact(4) * 5 = (fact(3) * 4) * 5 = ((fact(2) * 3) * 4) * 5 = (((fact(1) * 2) * 3) * 4) * 5 = ((1 * 2) * 3) * 4) * 5 = 120
    cout << fact(5) << endl;  // 输出: 120
}