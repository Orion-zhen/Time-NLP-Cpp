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

    for (auto item : solarMap)
    {
        string holi = item[0].asString();
        Date date;
        date.month = item[1].asInt();
        date.day = item[2].asInt();
        holi_solar.emplace(holi, date);
    }
    fin.close();

    fin.open(lunarPath, ios::in);
    if (!fin.is_open())
    {
        cout << "Error opening file" << endl;
        exit(1);
    }
    Json::Value lunarMap;
    if (!Json::parseFromStream(reader, fin, &lunarMap, &strerr))
    {
        cout << "Error opening file" << endl;
        exit(1);
    }

    for (auto item : lunarMap)
    {
        string holi = item[0].asString();
        Date date;
        date.month = item[1].asInt();
        date.day = item[2].asInt();
        holi_lunar.emplace(holi, date);
    }
    fin.close();
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

    Json::Value dic;
    if (timeToken.empty())
    {
        dic["error"] = "no time pattern could be extracted.";
    }
    else if (timeToken.size() == 1)
    {
        dic["type"] = "timestamp";
        ostringstream os;
        os << timeToken[0].Time.year << "-" << timeToken[0].Time.month << "-" << timeToken[0].Time.day << " " << timeToken[0].Time.hour << ":" << timeToken[0].Time.minute << ":" << timeToken[0].Time.second;
        dic["timestamp"] = os.str();
    }
    else
    {
        dic["type"] = "timespan";
        ostringstream os;
        os << timeToken[0].Time.year << "-" << timeToken[0].Time.month << "-" << timeToken[0].Time.day << " " << timeToken[0].Time.hour << ":" << timeToken[0].Time.minute << ":" << timeToken[0].Time.second;
        dic["timespan"].append(os.str());
        os.str("");
        os << timeToken[1].Time.year << "-" << timeToken[1].Time.month << "-" << timeToken[1].Time.day << " " << timeToken[1].Time.hour << ":" << timeToken[1].Time.minute << ":" << timeToken[1].Time.second;
        dic["timespan"].append(os.str());
    }

    Json::StreamWriterBuilder writer;
    writer["emitUTF8"] = true;
    writer["indentation"] = "";

    return Json::writeString(writer, dic);
}

void TimeNormalizer::__preHandling()
{
    StringPreHandler help;
    target = help.delKeyword(target, "\\s+");
    target = help.delKeyword(target, "[的]+");
    target = help.numberTranslator(target);
}

vector<TimeUnit> TimeNormalizer::__timeEx()
{
    int startline = -1;
    int endline = -1;
    int rpointer = 0;
    vector<string> tmp;

    sregex_iterator match(target.begin(), target.end(), pattern);
    for (; match != regexEnd; ++match)
    {
        startline = match->position();
        if (startline == endline)
        {
            --rpointer;
            tmp[rpointer] += match->str();
        }
        else
        {
            tmp.push_back(match->str());
        }
        endline = startline + match->length();
        ++rpointer; 
    }

    vector<TimeUnit> res;
    TimePoint contextTp;
    cout << timeBase << endl;

    for (size_t i = 0; i < size_t(rpointer); ++i)
    {
        res.push_back(TimeUnit(tmp[i], *this, contextTp));
        contextTp = res[i].tp;
    }

    return __filterTimeUnit(res);
}

vector<TimeUnit> TimeNormalizer::__filterTimeUnit(vector<TimeUnit> tu_arr)
{
    if (tu_arr.size() < 1)
    {
        return tu_arr;
    }
    vector<TimeUnit> res;
    for (auto elem : tu_arr)
    {
        if (elem.Time.formatTime != vector<int>{1970, 1, 1, 0, 0, 0})
        {
            res.push_back(elem);
        }
    }
    return res;
}