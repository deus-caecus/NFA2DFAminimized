#pragma once
#include"util.h"

void DFAminizing(const DFA& dfa, DFA& dfaMinimized);
void dividingSet(const DFA& dfa, std::set<SetStates>& equivalenceSet);