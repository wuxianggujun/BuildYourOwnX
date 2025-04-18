//
// Created by wuxianggujun on 2025/3/10.
//

#include <iostream>
#include <unordered_map>
#include <string>
#include <utility>

using std::cout;
using std::endl;

int main()
{
    std::unordered_map<std::string, float> book = {
        {"apple", 0.67},
        {"milk", 1.49},
        {"avocado", 1.49}

    };

    for (std::pair<std::string, float> pair : book)
    {
        cout << pair.first << ": " << pair.second << "$" << endl;
    }
}
