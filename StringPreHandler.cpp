// -*- coding: utf-8 -*-
#include "share.h"


using namespace std;

string StringPreHandler::delKeyword(string target, string rules)
{
	regex pattern(rules);
	return regex_replace(target, pattern, "");
}

string StringPreHandler::numberTranslator(string target)
{
	/*
	* 此处需要实现x万x，x千x等的识别，在python版本中由先行断言(?:)实现
	* 在c++中暂未找到实现方法
	*/

	regex pattern("[零一二两三四五六七八九]+");
	smatch match;
	if (regex_search(target, match, pattern))
		target = regex_replace(target, pattern, to_string(wordToNumber(match.str())), regex_constants::format_first_only);

	/*
	* 此处需要实现周x，星期x等的识别，在python版本中由先行断言(?<=)实现
	* 在c++中暂未找到实现方法
	*/

	pattern = regex("0?([1-9])百+([0-9])?([0-9])?");
	if (regex_search(target, match, pattern))
	{
		int num = 0;
		num += stoi(match.str(1)) * 100;
		num += (match.str(2).empty() ? 0 : stoi(match.str(2)) * 10);
		num += (match.str(3).empty() ? 0 : stoi(match.str(3)));
		target = regex_replace(target, pattern, to_string(num), regex_constants::format_first_only);
	}

	pattern = regex("0?([1-9])千+([0-9])?([0-9])?([0-9])?");
	if (regex_search(target, match, pattern))
	{
		int num = 0;
		num += stoi(match.str(1)) * 1000;
		num += (match.str(2).empty() ? 0 : stoi(match.str(2)) * 100);
		num += (match.str(3).empty() ? 0 : stoi(match.str(3)) * 10);
		num += (match.str(4).empty() ? 0 : stoi(match.str(4)));
		target = regex_replace(target, pattern, to_string(num), regex_constants::format_first_only);
	}

	pattern = regex("([0-9])+万+([0-9])?([0-9])?([0-9])?([0-9])?");
	if (regex_search(target, match, pattern))
	{
		int num = 0;
		num += (match.str(1).empty() ? 0 : stoi(match.str(1)) * 10000);
		num += (match.str(2).empty() ? 0 : stoi(match.str(2)) * 1000);
		num += (match.str(3).empty() ? 0 : stoi(match.str(3)) * 100);
		num += (match.str(4).empty() ? 0 : stoi(match.str(4)) * 10);
		num += (match.str(5).empty() ? 0 : stoi(match.str(5)));
		target = regex_replace(target, pattern, to_string(num), regex_constants::format_first_only);
	}

	return target;
}

int StringPreHandler::wordToNumber(string s)
{
	if (s == "零" || s == "0")
		return 0;
	else if (s == "一" || s == "1")
		return 1;
	else if (s == "二" || s == "两" || s == "2")
		return 2;
	else if (s == "三" || s == "3")
		return 3;
	else if (s == "四" || s == "4")
		return 4;
	else if (s == "五" || s == "5")
		return 5;
	else if (s == "六" || s == "6")
		return 6;
	else if (s == "七" || s == "天" || s == "日" || s == "7")
		return 7;
	else if (s == "八" || s == "8")
		return 8;
	else if (s == "九" || s == "9")
		return 9;
	else
		return -1;
}

int StringPreHandler::strToInt(string s)
{
	int res = 0;
	try
	{
		int res = stoi(s);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	return res;
}