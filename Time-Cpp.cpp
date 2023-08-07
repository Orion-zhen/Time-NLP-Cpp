// Time-Cpp.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "share.h"

using namespace std;

int main(void)
{
    TimeNormalizer tn;
    TimeStamp t = TimeStamp();

    string test = "晚上8点到上午10点之间";
    string res = tn.parse(test, t);

    cout << res << endl;

    return 0;
}
