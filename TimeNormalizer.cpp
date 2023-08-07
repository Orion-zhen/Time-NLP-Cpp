// -*- coding: utf-8 -*-
#include "share.h"

using namespace std;

static wstring_convert<codecvt_utf8<wchar_t>> converter;

string TimeNormalizer::_filter(string input_query)
{
    StringPreHandler help;
    input_query = help.numberTranslator(input_query);

    wstring winput_query = converter.from_bytes(input_query);
    wstring wrule = L"([0-9])月([0-9])";
    wregex pattern(wrule);
    wsmatch wmatch;
    if (regex_search(winput_query, wmatch, pattern))
    {
        wstring wrule1 = L"日|号";
        wregex pattern1(wrule1);
        wsmatch wmatch1;
        size_t pos = wmatch.str().find(L"月");
        wstring tmp = wmatch.str().substr(pos + 3);
        if (!regex_search(tmp, wmatch1, pattern1))
        {
            wstring wrule2 = L"([0-9])月([0-9]+)";
            wregex pattern2(wrule2);
            wsmatch wmatch2;
            if (regex_search(winput_query, wmatch2, pattern2))
            {
                wstring tmp = wmatch2.str();
                tmp = tmp.substr(0, tmp.find(L"月") + 3) + L"号";
                winput_query = regex_replace(winput_query, pattern2, tmp, regex_constants::format_first_only);
            }
        }
    }

    wrule = L"月";
    pattern = wregex(wrule);
    if (!regex_search(winput_query, wmatch, pattern))
    {
        winput_query = regex_replace(winput_query, wregex(L"个"), L"");
    }

    winput_query = regex_replace(winput_query, wregex(L"中旬"), L"15号");
    winput_query = regex_replace(winput_query, wregex(L"傍晚"), L"午后");
    winput_query = regex_replace(winput_query, wregex(L"大年"), L"");
    winput_query = regex_replace(winput_query, wregex(L"五一"), L"劳动节");
    winput_query = regex_replace(winput_query, wregex(L"白天"), L"早上");
    winput_query = regex_replace(winput_query, wregex(L"："), L":");

    input_query = converter.to_bytes(winput_query);
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
    // debug sentence
    cout << rule << endl;

    pattern = wregex(converter.from_bytes(rule));
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

string TimeNormalizer::parse(string _target, TimeStamp _timeBase)
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

    wstring wtarget = converter.from_bytes(target);
    wsregex_iterator match(wtarget.begin(), wtarget.end(), pattern);
    for (; match != regexEnd; ++match)
    {
        startline = match->position();
        if (startline == endline)
        {
            --rpointer;
            tmp[rpointer] += converter.to_bytes(match->str());
        }
        else
        {
            tmp.push_back(converter.to_bytes(match->str()));
        }
        endline = startline + match->length();
        ++rpointer; 
    }

    vector<TimeUnit> res;
    TimePoint contextTp;
    // cout << timeBase << endl;

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