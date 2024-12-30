/*
1.	自动机以状态转换矩阵的方式在计算机内部进行表示；
2.	根据NFA确定化的方法，将NFA确定化；
3.	判断自动机是否以最小化，若不是将其最小化；
4.	以状态转换矩阵的方法表示出确定化、最小化后的自动机。
*/

#include "NFA2DFA.h"
#include "DFAminizing.h"

int main()
{
	std::map<std::pair<StatesName, InputName>, std::set<StatesName>> NFAtransitionMap = {
		// q0 transitions
   {{"q0", Epsilon}, {"q1", "q2"}},   // epsilon-transition to q1 and q2

   // q1 transitions
   {{"q1", "a"}, {"q1", "q3"}},  // From q1 on 'a', transition to q1 and q3
   {{"q1", "b"}, {"q2"}},        // From q1 on 'b', transition to q2

   // q2 transitions
   {{"q2", "a"}, {"q3"}},        // From q2 on 'a', transition to q3
   {{"q2", "b"}, {"q1"}},        // From q2 on 'b', transition to q1

   // q3 transitions
   {{"q3", "a"}, {"q3"}},        // From q3 on 'a', stay in q3
   {{"q3", "b"}, {}}             // From q3 on 'b', no transitions (empty set)
	};
	StatesName start = "q0";
	std::set<StatesName> end = { "q3" };
	NFA nfa(NFAtransitionMap, start, end);
	DFA dfa;
	NFA2DFA(nfa, dfa);
	DFA minimizedDFA;
	DFAminizing(dfa, minimizedDFA);
	minimizedDFA.printTransitionMapName();
}