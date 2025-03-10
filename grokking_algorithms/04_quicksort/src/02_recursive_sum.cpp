//
// Created by wuxianggujun on 2025/3/9.
//

#include <iostream>
#include <vector>

using std::cout;
using std::endl;

template <typename T>
T sum(std::vector<T> arr)  // 值传递，函数内部修改不会影响原始数组
{
    if (arr.empty()) return 0;
    
    T last_num = arr.back();

    arr.pop_back();  // 现在可以修改arr了，因为它是一个副本

    return last_num + sum(arr);
}


int main()
{
    std::vector<int> arr_int = {1, 2, 3, 4};
    std::vector<float> arr_float = {0.1, 0.2, 0.3, 0.4, 0.5};

    cout << "Sum ints: " << sum(arr_int) << endl;
    cout << "Sum floats: " << sum(arr_float) << endl;
    return 0;
}
