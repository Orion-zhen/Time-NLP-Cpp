// -*- coding: utf-8 -*-
#include "share.h"

using namespace std;

static wstring_convert<codecvt_utf8<wchar_t>> converter;

string StringPreHandler::delKeyword(string target, string rules)
{
	wstring wrules = converter.from_bytes(rules);
	wstring wtarget = converter.from_bytes(target);
	wregex pattern(wrules);
	return converter.to_bytes(regex_replace(wtarget, pattern, L""));
}

string StringPreHandler::numberTranslator(string target)
{
	/*
	* 此处需要实现x万x，x千x等的识别，在python版本中由先行断言(?:)实现
	* 在c++中暂未找到实现方法
	*/
	wsregex_iterator wmatch;
	wstring wtarget = converter.from_bytes(target);

	wregex pattern(L"[零一二两三四五六七八九]+");
	for (wmatch = wsregex_iterator(wtarget.begin(), wtarget.end(), pattern); wmatch != regexEnd; ++wmatch)
		{
			string tmp = converter.to_bytes(wmatch->str());
			wtarget = regex_replace(wtarget, pattern, to_wstring(wordToNumber(tmp)), regex_constants::format_first_only);
		}
		
	/*
	* 此处需要实现周x，星期x等的识别，在python版本中由先行断言(?<=)实现
	* 在c++中暂未找到实现方法
	*/

	pattern = wregex(L"0?([1-9])百+([0-9])?([0-9])?");

	for (wmatch = wsregex_iterator(wtarget.begin(), wtarget.end(), pattern); wmatch != regexEnd; ++wmatch)
	{
		int num = 0;
		num += stoi(wmatch->str(1)) * 100;
		num += (wmatch->str(2).empty() ? 0 : stoi(wmatch->str(2)) * 10);
		num += (wmatch->str(3).empty() ? 0 : stoi(wmatch->str(3)));
		wtarget = regex_replace(wtarget, pattern, to_wstring(num), regex_constants::format_first_only);
	}

	pattern = wregex(L"0?([1-9])千+([0-9])?([0-9])?([0-9])?");
	for (wmatch = wsregex_iterator(wtarget.begin(), wtarget.end(), pattern); wmatch != regexEnd; ++wmatch)
	{
		int num = 0;
		num += stoi(wmatch->str(1)) * 1000;
		num += (wmatch->str(2).empty() ? 0 : stoi(wmatch->str(2)) * 100);
		num += (wmatch->str(3).empty() ? 0 : stoi(wmatch->str(3)) * 10);
		num += (wmatch->str(4).empty() ? 0 : stoi(wmatch->str(4)));
		wtarget = regex_replace(wtarget, pattern, to_wstring(num), regex_constants::format_first_only);
	}

	pattern = wregex(L"([0-9])+万+([0-9])?([0-9])?([0-9])?([0-9])?");
	for (wmatch = wsregex_iterator(wtarget.begin(), wtarget.end(), pattern); wmatch != regexEnd; ++wmatch)
	{
		int num = 0;
		num += (wmatch->str(1).empty() ? 0 : stoi(wmatch->str(1)) * 10000);
		num += (wmatch->str(2).empty() ? 0 : stoi(wmatch->str(2)) * 1000);
		num += (wmatch->str(3).empty() ? 0 : stoi(wmatch->str(3)) * 100);
		num += (wmatch->str(4).empty() ? 0 : stoi(wmatch->str(4)) * 10);
		num += (wmatch->str(5).empty() ? 0 : stoi(wmatch->str(5)));
		wtarget = regex_replace(wtarget, pattern, to_wstring(num), regex_constants::format_first_only);
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