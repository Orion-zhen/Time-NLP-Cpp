// -*- coding: utf-8 -*-
#include "share.h"

using namespace std;

int GetBitInt(int data, int length, int shift)
{
    return (data & (((1 << length) - 1) << shift)) >> shift;
}

unsigned int SolarToInt(int y, int m, int d)
{
    m = (m + 9) % 12;
    y = y - m / 10;
    return 365 * y + y / 4 - y / 100 + y / 400 + (m * 306 + 5) / 10
        + (d - 1);
}

Solar SolarFromInt(unsigned int g)
{
    int y = (10000 * g + 14780) / 3652425;
    int ddd = g - (365 * y + y / 4 - y / 100 + y / 400);
    if (ddd < 0)
    {
        y--;
        ddd = g - (365 * y + y / 4 - y / 100 + y / 400);
    }
    int mi = (100 * ddd + 52) / 3060;
    int mm = (mi + 2) % 12 + 1;
    y = y + (mi + 2) / 12;
    int dd = ddd - (mi * 306 + 5) / 10 + 1;
    Solar solar(y, mm, dd);
    return solar;
}

Solar LunarSolarConverter::LunarToSolar(const Lunar& lunar)
{
    int days = lunar_month_days[lunar.lunarYear - lunar_month_days[0]];
    int leap = GetBitInt(days, 4, 13);
    int offset = 0;
    int loopend = leap;
    if (!lunar.isleap)
    {
        if (lunar.lunarMonth <= leap || leap == 0)
        {
            loopend = lunar.lunarMonth - 1;
        }
        else
        {
            loopend = lunar.lunarMonth;
        }
    }
    for (int i = 0; i < loopend; i++)
    {
        offset += GetBitInt(days, 1, 12 - i) == 1 ? 30 : 29;
    }
    offset += lunar.lunarDay;

    int solar11 = solar_1_1[lunar.lunarYear - solar_1_1[0]];

    int y = GetBitInt(solar11, 12, 9);
    int m = GetBitInt(solar11, 4, 5);
    int d = GetBitInt(solar11, 5, 0);

    return SolarFromInt(SolarToInt(y, m, d) + offset - 1);
}

Lunar LunarSolarConverter::SolarToLunar(const Solar& solar)
{
    Lunar lunar;
    int index = solar.solarYear - solar_1_1[0];
    int data = (solar.solarYear << 9) | (solar.solarMonth << 5)
        | (solar.solarDay);
    int solar11 = 0;
    if (solar_1_1[index] > data)
    {
        index--;
    }
    solar11 = solar_1_1[index];
    int y = GetBitInt(solar11, 12, 9);
    int m = GetBitInt(solar11, 4, 5);
    int d = GetBitInt(solar11, 5, 0);
    long offset = SolarToInt(solar.solarYear, solar.solarMonth,
        solar.solarDay) - SolarToInt(y, m, d);

    int days = lunar_month_days[index];
    int leap = GetBitInt(days, 4, 13);

    int lunarY = index + solar_1_1[0];
    int lunarM = 1;
    int lunarD = 1;
    offset += 1;

    for (int i = 0; i < 13; i++)
    {
        int dm = GetBitInt(days, 1, 12 - i) == 1 ? 30 : 29;
        if (offset > dm)
        {
            lunarM++;
            offset -= dm;
        }
        else
        {
            break;
        }
    }
    lunarD = (int)offset;
    lunar.lunarYear = lunarY;
    lunar.lunarMonth = lunarM;
    lunar.isleap = false;
    if (leap != 0 && lunarM > leap)
    {
        lunar.lunarMonth = lunarM - 1;
        if (lunarM == leap + 1)
        {
            lunar.isleap = true;
        }
    }

    lunar.lunarDay = lunarD;
    return lunar;
}