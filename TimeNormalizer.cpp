// -*- coding: utf-8 -*-
#include "share.h"

using namespace std;

string TimeNormalizer::_filter(string input_query)
{
    StringPreHandler help;
    input_query = help.numberTranslator(input_query);
}