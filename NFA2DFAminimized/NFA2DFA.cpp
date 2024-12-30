#include "NFA2DFA.h"

void NFA2DFA(const NFA& nfa, DFA& dfa)
{
	std::set<StatesID> startSet;
	startSet.insert(nfa.start);
	SetStates startState = nfa.getEpsilonClosure(startSet);//Start symbol's Epslion closure
	dfa.start = dfa.states.addState(startState);//definite it as start symbol
	dfa.inputs = nfa.inputs;
	auto it = dfa.inputs.stateID_Bimap.left.find(Epsilon);
	if (it != dfa.inputs.stateID_Bimap.left.end())
	{
		dfa.inputs.stateID_Bimap.left.erase(it);
	}
	dfa.nameID = nfa.states;
	std::set<SetStates> allSetStates;
	std::set<SetStates> currentSetStates;
	std::set<SetStates> newAddSetStates;
	allSetStates.insert(startState);
	currentSetStates.insert(startState);
	while (!currentSetStates.empty())
	{
		
		for (const auto& currentState : currentSetStates)
		{
			newAddSetStates.clear();
			for (const auto& [inputName, input] : dfa.inputs.stateID_Bimap.left)
			{
				SetStates sourceState = nfa.getEpsilonClosure(currentState);
				SetStates targetState = nfa.getNextStates(sourceState, input);

				if (targetState.empty())
				{
					continue;
				}
				StatesID targetID;
				StatesID sourceID;
				if (allSetStates.find(targetState) == allSetStates.end())
				{
					newAddSetStates.insert(targetState);
				}

				sourceID = dfa.states.addState(sourceState);
				targetID = dfa.states.addState(targetState);
				if (setsHaveIntersect(targetState, nfa.end))
				{
					dfa.end.insert(targetID);
				}
				auto pair = std::make_pair(sourceID, input);
				dfa.transitionMapID[pair] = targetID;
			}
		}
		//分段式处理新增状态
		allSetStates.insert(newAddSetStates.begin(), newAddSetStates.end());
		currentSetStates = newAddSetStates;
	}
#if DEBUG
	dfa.printTransitionMapName();
#endif
}
