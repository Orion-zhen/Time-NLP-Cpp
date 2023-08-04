// -*- coding: utf-8 -*-
#include "share.h"

using namespace std;

string TimeNormalizer::_filter(string input_query)
{
    StringPreHandler help;
    input_query = help.numberTranslator(input_query);

    string rule = "([0-9])月([0-9])";
    regex pattern(rule);
    smatch match;
    if (regex_search(input_query, match, pattern))
    {
        string rule1 = "日|号";
        regex pattern1(rule1);
        smatch match1;
        size_t pos = match.str().find("月");
        string tmp = match.str().substr(pos + 3);
        if (!regex_search(tmp, match1, pattern1))
        {
            string rule2 = "([0-9])月([0-9]+)";
            regex pattern2(rule2);
            smatch match2;
            if (regex_search(input_query, match2, pattern2))
            {
                string tmp = match2.str();
                tmp = tmp.substr(0, tmp.find("月") + 3) + "号";
                input_query = regex_replace(input_query, pattern2, tmp, regex_constants::format_first_only);
            }
        }
    }

    rule = "月";
    pattern = regex(rule);
    if (!regex_search(input_query, match, pattern))
    {
        input_query = regex_replace(input_query, regex("个"), "");
    }

    input_query = regex_replace(input_query, regex("中旬"), "15号");
    input_query = regex_replace(input_query, regex("傍晚"), "午后");
    input_query = regex_replace(input_query, regex("大年"), "");
    input_query = regex_replace(input_query, regex("五一"), "劳动节");
    input_query = regex_replace(input_query, regex("白天"), "早上");
    input_query = regex_replace(input_query, regex("："), ":");

    return input_query;
}

void TimeNormalizer::init()
{
    string regexPath = "../resource/regex.txt";
    string solarPath = "../resource/holi_solar.json";
    string lunarPath = "../resource/holi_lunar.json";
    ifstream fin;

    fin.open(regexPath, ios::in);
    if (!fin.is_open())
    {
        cout << "Error opening file" << endl;
        exit(1);
    }
    string rule;
    fin >> rule;
    pattern = regex(rule);
    fin.close();

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
}

string TimeNormalizer::parse(string _target, TimeStamp _timeBase = TimeStamp())
{
    isTimeSpan = false;
    invalidSpan = false;
    timeSpan = "";
    target = _filter(_target);
    timeBase = _timeBase;
    nowTime = _timeBase;
    oldTimeBase = timeBase;
    __preHandling();
    timeToken = __timeEx();
}