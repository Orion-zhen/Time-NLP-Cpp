// -*- coding: utf-8 -*-
#include "share.h"

using namespace std;

void TimeUnit::time_normalization()
{
    norm_setyear();
    norm_setmonth();
    norm_setday();
    norm_setmonth_fuzzyday();
    norm_setCurRelated();
    norm_sethour();
    norm_setminute();
    norm_setsecond();
    norm_setSpecial();
    norm_setSpanRelated();
    norm_setHoliday();
    modifyTimeBase();
    tp_origin.tunit = tp.tunit;

    bool flag = true;
    for (size_t i = 0; i < 4; ++i)
    {
        if (tp.tunit[i] != -1)
        {
            flag = false;
            break;
        }
    }
    if (flag)
    {
        normalizer.isTimeSpan = true;
    }

    if (normalizer.isTimeSpan)
    {
        int days = 0;
        if (tp.tunit[0] > 0)
        {
            days += tp.tunit[0] * 365;
        }
        if (tp.tunit[1] > 0)
        {
            days += tp.tunit[1] * 30;
        }
        if (tp.tunit[2] > 0)
        {
            days += tp.tunit[2];
        }
        vector<int> tunit = tp.tunit;
        for (size_t i = 3; i < 6; ++i)
        {
            if (tp.tunit[i] < 0)
            {
                tunit[i] = 0;
            }
        }
        int seconds = tunit[3] * 3600 + tunit[4] * 60 + tunit[5];
        if (seconds == 0 && days == 0)
        {
            normalizer.invalidSpan = true;
        }
        else
        {
            normalizer.timeSpan = genSpan(days, seconds);
        }
        return;
    }

    size_t tunitPointer = 5;
    while (tunitPointer >= 0 && tp.tunit[tunitPointer] < 0)
    {
        --tunitPointer;
    }
    for (size_t i = 0; i < tunitPointer; ++i)
    {
        if (tp.tunit[i] < 0)
        {
            tp.tunit[i] = int(normalizer.timeBase.formatTime[i]);
        }
    }

    Time = genTime(tp.tunit);
}

string TimeUnit::genSpan(int days, int seconds)
{
    int day = int(seconds / (24 * 3600));
    int h = int((seconds % (24 * 3600)) / 3600);
    int m = int((seconds % (24 * 3600) % 3600) / 60);
    int s = int((seconds % (24 * 3600) % 3600) % 60);

    ostringstream os;
    os << days + day << " days, " << h << ":" << m << ":" << s;
    return os.str();
}

TimeStamp TimeUnit::genTime(const TimeFormat& tunit)
{
    return TimeStamp(tunit);
}

void TimeUnit::norm_setyear()
{
    // 此处有先向断言
    string rule = "(\\d){1}年+";
    regex pattern(rule);
    smatch match;
    if (regex_search(exp_time, match, pattern))
    {
        normalizer.isTimeSpan = true;
        tp.tunit[0] = stoi(match.str());
    }

    rule = "(\\d){2}年+";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        tp.tunit[0] = stoi(match.str());
    }

    // 此处有先向断言
    rule = "(\\d){3}年+";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        normalizer.isTimeSpan = true;
        tp.tunit[0] = stoi(match.str());
    }

    rule = "(\\d){4}年+";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        tp.tunit[0] = stoi(match.str());
    }
}

void TimeUnit::norm_setmonth()
{
    // 此处有先向断言
    string rule = "(10|11|12|[1-9])月+";
    regex pattern(rule);
    smatch match;
    if (regex_search(exp_time, match, pattern))
    {
        tp.tunit[1] = stoi(match.str());
        preferFuture(1);
    }
}

void TimeUnit::norm_setmonth_fuzzyday()
{
    string rule = "((10)|(11)|(12)|([1-9]))[月|\\.|\\-]([0-3][0-9]|[1-9])";
    regex pattern(rule);
    smatch match;
    if (regex_search(exp_time, match, pattern))
    {
        tp.tunit[1] = stoi(match.str(1));
        tp.tunit[2] = stoi(match.str(2));
        preferFuture(1);
        _check_time(tp.tunit);
    }
}

void TimeUnit::norm_setday()
{
    
    // 此处有先向断言
    string rule = "([0-3][0-9]|[1-9])[日|号]+";
    regex pattern(rule);
    smatch match;
    if (regex_search(exp_time, match, pattern))
    {
        tp.tunit[2] = stoi(match.str());
        preferFuture(2);
        _check_time(tp.tunit);
    }
    
}

void TimeUnit::norm_checkKeyword()
{
    /*
    * 对关键字：早（包含早上/早晨/早间），上午，中午,午间,下午,午后,晚上,傍晚,晚间,晚,pm,PM的正确时间计算
    * 规约：
    * 1.中午/午间0-10点视为12-22点
    * 2.下午/午后0-11点视为12-23点
    * 3.晚上/傍晚/晚间/晚1-11点视为13-23点，12点视为0点
    * 4.0-11点pm/PM视为12-23点
    */
    string rule = "凌晨";
    regex pattern(rule);
    smatch match;
    if (regex_search(exp_time, match, pattern))
    {
        isMorning = true;
        if (tp.tunit[3] == -1)
        {
            tp.tunit[3] = timeRange.dayBreak;
        }
        else if (tp.tunit[3] >= 12 && tp.tunit[3] <= 23)
        {
            tp.tunit[3] -= 12;
        }
        else if (tp.tunit[3] == 0)
        {
            tp.tunit[3] = 12;
        }
        preferFuture(3);
        isAllDayTime = false;
    }

    rule = "早上|早晨|早间|晨间|今早|明早|早|清晨";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        isMorning = true;
        if (tp.tunit[3] == -1)
        {
            tp.tunit[3] = timeRange.earlyMorning;
        }
        else if (tp.tunit[3] >= 12 && tp.tunit[3] <= 23)
        {
            tp.tunit[3] -= 12;
        }
        else if (tp.tunit[3] == 0)
        {
            tp.tunit[3] = 12;
        }
        preferFuture(3);
        isAllDayTime = false;
    }

    rule = "上午";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        isMorning = true;
        if (tp.tunit[3] == -1)
        {
            tp.tunit[3] = timeRange.morning;
        }
        else if (tp.tunit[3] >= 12 && tp.tunit[3] <= 23)
        {
            tp.tunit[3] -= 12;
        }
        else if (tp.tunit[3] == 0)
        {
            tp.tunit[3] = 12;
        }
        preferFuture(3);
        isAllDayTime = false;
    }

    rule = "中午|午间|今午|明午|白天|AM|am";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        isMorning = false;
        if (tp.tunit[3] == -1)
        {
            tp.tunit[3] = timeRange.noon;
        }
        else if (tp.tunit[3] >= 0 && tp.tunit[3] <= 10)
        {
            tp.tunit[3] += 12;
        }
        else if (tp.tunit[3] == 12)
        {
            tp.tunit[3] = 0;
        }
        preferFuture(3);
        isAllDayTime = false;
    }

    rule = "下午|午后|pm|PM|今晚|明晚";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        isMorning = false;
        if (tp.tunit[3] == -1)
        {
            tp.tunit[3] = timeRange.afternoon;
        }
        else if (tp.tunit[3] >= 0 && tp.tunit[3] <= 11)
        {
            tp.tunit[3] += 12;
        }
        else if (tp.tunit[3] == 12)
        {
            tp.tunit[3] = 0;
        }
        preferFuture(3);
        isAllDayTime = false;
    }
    
    rule = "晚上|夜间|夜里|今夜|明夜|晚|夜里|今晚";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        isMorning = false;
        if (tp.tunit[3] == -1)
        {
            tp.tunit[3] = timeRange.night;
        }
        else if (tp.tunit[3] >= 1 && tp.tunit[3] <= 11)
        {
            tp.tunit[3] += 12;
        }
        else if (tp.tunit[3] == 12)
        {
            tp.tunit[3] = 0;
        }
        preferFuture(3);
        isAllDayTime = false;
    }
}

void TimeUnit::norm_sethour()
{
    // 此处有先向断言
    string rule = "([0-2]?[0-9])(点|时)";
    regex pattern(rule);
    smatch match;
    if (regex_search(exp_time, match, pattern))
    {
        tp.tunit[3] = stoi(match.str(1));
        preferFuture(3);
        isAllDayTime = false;
    }
    norm_checkKeyword();
}

void TimeUnit::norm_setminute()
{
    // 此处有先向断言
    
}

void TimeUnit::norm_setsecond()
{
    // 此处有先向断言
}

void TimeUnit::norm_setSpecial()
{
    // 此处有先向断言
}

void TimeUnit::norm_setBaseRelated()
{
    // 此处有先向断言
}

void TimeUnit::norm_setSpanRelated()
{
    // 此处有先向断言
}

void TimeUnit::norm_setHoliday()
{
    string rule = "(情人节)|(母亲节)|(青年节)|(教师节)|(中元节)|(端午)|(劳动节)|(7夕)|(建党节)|(建军节)|\
    (初13)|(初14)|(初15)|(初12)|(初11)|(初9)|(初8)|(初7)|(初6)|(初5)|(初4)|(初3)|(初2)|(初1)|\
    (中和节)|(圣诞)|(中秋)|(春节)|(元宵)|(航海日)|(儿童节)|(国庆)|(植树节)|(元旦)|(重阳节)|(妇女节)|(记者节)|\
    (立春)|(雨水)|(惊蛰)|(春分)|(清明)|(谷雨)|\
    (立夏)|(小满 )|(芒种)|(夏至)|(小暑)|(大暑)|\
    (立秋)|(处暑)|(白露)|(秋分)|(寒露)|(霜降)|\
    (立冬)|(小雪)|(大雪)|(冬至)|(小寒)|(大寒)";
    regex pattern(rule);
    smatch match;
    if (regex_search(exp_time, match, pattern))
    {
        if (tp.tunit[0] == -1)
        {
            tp.tunit[0] = normalizer.timeBase.year;
        }
        string holi = match.str();

        if (holi.find("节") != string::npos)
        {
            Date date;
            if (normalizer.holi_solar.find(holi) != normalizer.holi_solar.end())
            {
                date = normalizer.holi_solar[holi];
            }
            else if (normalizer.holi_lunar.find(holi) != normalizer.holi_lunar.end())
            {
                date = normalizer.holi_lunar[holi];
                Lunar lunar(tp.tunit[0], date.month, date.day, false);
                Solar solar = LunarSolarConverter()(lunar);
                tp.tunit[0] = solar.solarYear;
                date.month = solar.solarMonth;
                date.day = solar.solarDay;
            }
        }
        else
        {
            // 24节气难以实现
            // call china_24_st()
        } 
    }
}

void TimeUnit::norm_setCurRelated()
{
    TimeStamp cur = normalizer.timeBase;
    vector<bool> flag {false, false, false};

    string rule = "前年";
    regex pattern(rule);
    smatch match;
    if (regex_search(exp_time, match, pattern))
    {
        cur.year -= 2;
        flag[0] = true;
    }

    rule = "去年";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        cur.year -= 1;
        flag[0] = true;
    }

    rule = "今年";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        flag[0] = true;
    }

    rule = "明年";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        cur.year += 1;
        flag[0] = true;
    }

    rule = "后年";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        cur.year += 2;
        flag[0] = true;
    }

    rule = "([上]*)个?月";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        // 一个中文string的长度为3
        int month = match.str(1).length() / 3;
        cur.month -= month;
        flag[1] = true;
    }

    rule = "(本|这个)月";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        flag[1] = true;
    }

    rule = "([下]*)个?月";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        // 一个中文string的长度为3
        int month = match.str(1).length() / 3;
        cur.month += month;
        flag[1] = true;
    }

    rule = "([大]*)前天";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        // 一个中文string的长度为3
        int day = match.str(1).length() / 3 + 2;
        cur.day -= day;
        flag[2] = true;
    }

    rule = "昨天";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        cur.day -= 1;
        flag[2] = true;
    }

    /*
    * 除了今年、明年的情况，
    * 今x，明x需要用到先向断言
    */

    rule = "([大]*)后天";
    pattern = regex(rule);
    if (regex_search(exp_time, match, pattern))
    {
        // 一个中文string的长度为3
        int day = match.str(1).length() / 3 + 2;
        cur.day += day;
        flag[2] = true;
    }

    /*
    * 之后是对星期的匹配，也是用的先向断言
    * c++ regex难以解决
    */

    if (flag[0] || flag[1] || flag[2])
    {
        tp.tunit[0] = cur.year;
    }
    if (flag[1] || flag[2])
    {
        tp.tunit[1] = cur.month;
    }
    if (flag[2])
    {
        tp.tunit[2] = cur.day;
    } 
}

void TimeUnit::modifyTimeBase()
{
    if (!normalizer.isTimeSpan)
    {
        // 此处将两位数的年份转换为四位数的年份
        // 原python代码仅仅是将小于30的年份转换为20xx
        // 此处优化为将小于当前年份的值转换为20xx，大于当前年份的值转换为19xx
        TimeStamp nowTime = TimeStamp();
        if (tp.tunit[0] > (nowTime.year - 2000) && tp.tunit[0] < 100)
        {
            tp.tunit[0] += 1900;
        }
        else if (tp.tunit[0] < (nowTime.year - 2000) && tp.tunit[0] > 0)
        {
            tp.tunit[0] += 2000;
        }

        TimeStamp time_grid = normalizer.timeBase;
        for (size_t i = 0; i < 6; ++i)
        {
            if (tp.tunit[i] != -1)
            {
                time_grid.formatTime[i] = tp.tunit[i];
            }
        }
        time_grid.rebuild();
        normalizer.timeBase = time_grid;
    }
}

TimeStamp TimeUnit::preferFutureWeek(int weekday, TimeStamp cur)
{
    if (!normalizer.isPreferFuture)
    {
        return cur;
    }

    for (size_t i = 0; i < 2; ++i)
    {
        if (tp.tunit[i] != -1)
        {
            return cur;
        }
    }

    TimeStamp tmp = TimeStamp();
    if (tmp.wday > weekday)
    {
        tmp.day += 7;
    }
    return tmp;
}

void TimeUnit::preferFuture(size_t timeIndex)
{
    for (size_t i = 0; i < timeIndex; ++i)
    {
        if (tp.tunit[i] != -1)
        {
            return;
        }
    }

    checkContextTime(timeIndex);

    if (!normalizer.isPreferFuture)
    {
        return;
    }

    TimeStamp time_arr = normalizer.timeBase;
    TimeStamp cur = TimeStamp();

    int cur_unit = time_arr.formatTime[timeIndex];

    if (tp.tunit[0] == -1)
    {
        _noyear = true;
    }
    else
    {
        _noyear = false;
    }

    if (cur_unit < tp.tunit[timeIndex])
    {
        return;
    }

    cur = addTime(cur, timeIndex - 1);
    time_arr = cur;
    for (size_t i = 0; i < timeIndex; ++i)
    {
        tp.tunit[i] = time_arr.formatTime[i];
    }
}

void TimeUnit::_check_time(TimeFormat parse)
{
    TimeStamp time_arr = normalizer.timeBase;
    if (_noyear)
    {
        if (parse[1] == time_arr.formatTime[1])
        {
            if (parse[2] > time_arr.formatTime[2])
            {
                --parse[0];
            }
        }
        _noyear = false;
    }
}

void TimeUnit::checkContextTime(size_t timeIndex)
{
    if (!isFirstTimeSolveContext)
    {
        return;
    }

    for (size_t i = 0; i < timeIndex; ++i)
    {
        if (tp.tunit[i] == -1 && tp_origin.tunit[i] != -1)
        {
            tp.tunit[i] = tp_origin.tunit[i];
        }
    }

    int t_o = tp_origin.tunit[timeIndex];
    int t = tp.tunit[timeIndex];

    if (!isMorning && timeIndex == 3 && t_o >= 12 && t < 12 && t > (t_o - 12))
    {
        tp.tunit[3] += 12;
    }
    isFirstTimeSolveContext = false; 
}

TimeStamp TimeUnit::addTime(TimeStamp cur, size_t fore_unit)
{
    if (tp.tunit[fore_unit] != -1)
    {
        return cur;
    }

    switch (fore_unit)
    {
    case 0:
        cur.year += 1;
        break;
    case 1:
        cur.month += 1;
        break;
    case 2:
        cur.day += 1;
        break;
    case 3:
        cur.hour += 1;
        break;
    case 4:
        cur.minute += 1;
        break;
    case 5:
        cur.second += 1;
        break;
    default:
        break;
    }
    cur.reformat();
    return cur;
}