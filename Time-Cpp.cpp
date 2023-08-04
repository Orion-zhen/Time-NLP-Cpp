// Time-Cpp.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <regex>
#include <string>

using namespace std;

int main()
{
    regex pattern("([常]*)在");
    smatch match;
    string test = "在";
    string tmp = "常";
    if (regex_search(test, match, pattern))
        cout << match.str(1).length() << endl;
}
