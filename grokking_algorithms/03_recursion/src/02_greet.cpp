//
// Created by wuxianggujun on 2025/3/9.
//

#include <iostream>
#include <string>

using std::cout;
using std::endl;

void greet2(std::string name)
{
    cout << "How are you, " << name << "?" << endl;
}

void bye()
{
    cout << "OK, bye!" << endl;
}

void greet(std::string name)
{
    cout << "Hello,"+name+"!" << endl;
    greet2(name);
    cout << "Getting ready to say bye..." << endl;
}

int main()
{
    greet("Adit");
    
    return 0;
}