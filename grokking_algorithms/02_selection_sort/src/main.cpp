//
// Created by wuxianggujun on 2025/3/9.
//
#include <iostream>
#include <vector>
#include <algorithm>

// 选择排序
using std::cout;
using std::endl;

// template <typename T>
// int find_smallest(const std::vector<T>& arr)
// {
//     T smallest = arr[0];
//
//     int smallest_index = 0;
//
//     for (int i = 0; i < arr.size(); i++)
//     {
//         if (arr[i] < smallest)
//         {
//             smallest = arr[i];
//             smallest_index = i;
//         }
//     }
//
//     return smallest_index;
// }
//
// template <typename T>
// std::vector<T> selection_sort(std::vector<T> arr)
// {
//     std::vector<T> sorted;
//
//     while (!arr.empty())
//     {
//         int smallest_index = find_smallest(arr);
//         sorted.push_back(arr[smallest_index]);
//         arr.erase(arr.begin() + smallest_index);
//     }
//     return sorted;
// }

template <typename T>
void selection_sort(std::vector<T>& arr)
{
    /*
     * 选择排序内存操作动画
     * 
     * 初始数组:
     * [1.2] [1.0] [3.0] [0.0] [-1.0] [0.5] [100] [-99]
     *   0     1     2     3      4      5     6     7
     */
    for(size_t i = 0; i < arr.size()-1; ++i) {
        size_t min_index = i;
        
        /*
         * 第i轮查找最小值
         * i = 当前轮次
         * ^ = 当前位置(i)
         * * = 当前找到的最小值位置(min_index)
         * > = 正在比较的元素(j)
         */
        
        // 只需找出后续最小元素的索引
        for(size_t j = i+1; j < arr.size(); ++j) {
            if(arr[j] < arr[min_index]) {
                min_index = j;
            }
            /*
             * 比较过程 (以第一轮i=0为例):
             * [1.2] [1.0] [3.0] [0.0] [-1.0] [0.5] [100] [-99]
             *   ^>    >     >     >      *      >     >     >
             */
        }
        
        // 避免不必要的交换
        if(min_index != i) {
            std::swap(arr[i], arr[min_index]);
            /*
             * 交换操作 (以第一轮i=0为例):
             * 交换前:
             * [1.2] [1.0] [3.0] [0.0] [-1.0] [0.5] [100] [-99]
             *   ^                        *
             * 交换后:
             * [-1.0] [1.0] [3.0] [0.0] [1.2] [0.5] [100] [-99]
             *    ^                       *
             */
        }
        
        /*
         * 每轮排序后的数组状态:
         * 第1轮: [-1.0] [1.0] [3.0] [0.0] [1.2] [0.5] [100] [-99]
         *          √
         * 第2轮: [-1.0] [-99] [3.0] [0.0] [1.2] [0.5] [100] [1.0]
         *          √     √
         * 第3轮: [-1.0] [-99] [0.0] [3.0] [1.2] [0.5] [100] [1.0]
         *          √     √     √
         * 第4轮: [-1.0] [-99] [0.0] [0.5] [1.2] [3.0] [100] [1.0]
         *          √     √     √     √
         * 第5轮: [-1.0] [-99] [0.0] [0.5] [1.0] [3.0] [100] [1.2]
         *          √     √     √     √     √
         * 第6轮: [-1.0] [-99] [0.0] [0.5] [1.0] [1.2] [100] [3.0]
         *          √     √     √     √     √     √
         * 第7轮: [-1.0] [-99] [0.0] [0.5] [1.0] [1.2] [3.0] [100]
         *          √     √     √     √     √     √     √
         * 
         * 最终排序结果:
         * [-99] [-1.0] [0.0] [0.5] [1.0] [1.2] [3.0] [100]
         *   0      1     2     3     4     5     6     7
         */
    }
}

int main()
{
    std::vector<float> arr = {1.2, 1.0, 3, 0, -1.0, 0.5, 100, -99};
    selection_sort(arr);

    cout << "Sorted array: ";
    for (const float num : arr)
    {
        cout << num << " ";
    }
    cout << endl;
}
