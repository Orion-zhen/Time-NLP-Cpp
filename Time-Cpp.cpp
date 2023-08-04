// Time-Cpp.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <fstream>
#include <sstream>
#include "json.h"

#include <regex>
#include <string>

using namespace std;

struct Date
{
	int month = 0;
	int day = 0;
};

int main()
{
    string regexPath = "../resource/regex.txt";
    string solarPath = "../resource/holi_solar.json";
    string lunarPath = "../resource/holi_lunar.json";
    ifstream fin;

    fin.open(solarPath, ios::in);
    if (!fin.is_open())
    {
        cout << "Error opening file" << endl;
        exit(1);
    }
    Json::CharReaderBuilder reader;
    reader["emitUTF8"] = true;
    Json::Value solarMap;
    string strerr;
    if (!Json::parseFromStream(reader, fin, &solarMap, &strerr))
    {
        cout << "Error opening file" << endl;
        exit(1);
    }
    cout << solarMap[0][1] << endl; 
}
