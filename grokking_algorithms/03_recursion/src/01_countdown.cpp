//
// Created by wuxianggujun on 2025/3/9.
//

#include <iostream>

using std::cout;
using std::endl;

void countdown(const int& i)
{
    cout << i << endl;

    if (i == 0) return;

    countdown(i-1);
}



int main()
{
    countdown(5);
}