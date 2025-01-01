/*
1.	自动机以状态转换矩阵的方式在计算机内部进行表示；
2.	根据NFA确定化的方法，将NFA确定化；
3.	判断自动机是否以最小化，若不是将其最小化；
4.	以状态转换矩阵的方法表示出确定化、最小化后的自动机。
*/
#include"readingExcel.h"
#include "NFA2DFA.h"
#include "DFAminizing.h"

int main()
{
	TransitionMap NFAtransitionMap;
	StatesName start;
	std::set<StatesName> end;
	std::wstring  wfilePath= openExcelFile();
	processFile(wfilePath,NFAtransitionMap,start,end);

	NFA nfa(NFAtransitionMap, start, end);
	DFA dfa;
	NFA2DFA(nfa, dfa);
	DFA minimizedDFA;
	DFAminizing(dfa, minimizedDFA);
	minimizedDFA.printTransitionMapName();
}