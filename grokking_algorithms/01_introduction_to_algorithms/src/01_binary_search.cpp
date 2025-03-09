//
// Created by wuxianggujun on 2025/3/9.
//
#include <iostream>
#include <vector>

// 二分查找法的两种不同实现
using std::cout;
using std::endl;

// 在有序数组中通过索引二分查找目标值
template <typename T>
int binary_search(const std::vector<T>& list, const T& item)
{
    int low = 0;
    int high = list.size() - 1;

    while (low <= high)
    {
        int mid = (low + high) / 2;
        T guess = list[mid];
        if (guess == item)
        {
            return mid;
        }

        if (guess > item)
        {
            high = mid - 1;
        }
        else
        {
            low = mid + 1;
        }
    }
    return -1;
}

// 通过指针直接操作内存地址进行二分查找
template <typename T>
const T* binary_search2(const std::vector<T>& list, const T& item)
{
    const T* low = &list.front();
    const T* high = &list.back();

    while (low <= high)
    {
        const T* guess = low + ((high - low) / 2);
        if (*guess == item)
            return guess;

        if (*guess > item)
        {
            high = guess - 1;
        }
        else
        {
            low = guess + 1;
        }
    }
    return nullptr;
}


/*
 *
 *地址: 0x1000   0x1004   0x1008   0x100C   0x1010
 *值 : [  1   ] [  3   ] [  5   ] [  7   ] [  9   ]
       ↑low                ↑guess            ↑high
       */



int main()
{
    std::vector<int> my_list = {1,3,5,7,9};
    const int * binary_search2_result = binary_search2(my_list, 9);
    const int * binary_search2_null = binary_search2(my_list, 4);

    cout << "Binary search for number 3:" << binary_search(my_list, 3) << endl;
    cout << "Binary search for number 9 (memory address):" << binary_search2_result << endl;
    cout << "Binary search for number 9 (value):" << binary_search2_result << endl;

    if (binary_search2_null == nullptr)
    {
        cout << "4 was not found in the list" << endl;
    }

    return 0;
}
