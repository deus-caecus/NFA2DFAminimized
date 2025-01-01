#pragma once
#include"util.h"
#include<sstream>
#include<xlnt/xlnt.hpp>
#include<windows.h>
#include<commdlg.h>
#include<cstdlib>
std::wstring openExcelFile();

void processFile(const std::wstring& filePath, TransitionMap& transitionMap, StatesName& start, std::set<StatesName>& end);
