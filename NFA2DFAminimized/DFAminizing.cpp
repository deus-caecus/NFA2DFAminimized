#include "DFAminizing.h"


void DFAminizing(const DFA& dfa, DFA& dfaMinimized)
{
	dfaMinimized.nameID = dfa.nameID;
	dfaMinimized.inputs = dfa.inputs;
	std::set<StatesID> nonTerminal;
	std::set<StatesID> terminal;
	terminal = dfa.end;
	nonTerminal = dfa.states.usedID;
	for (const StatesID& elem : terminal)
	{
		nonTerminal.erase(elem);
	}
	std::set<SetStates> equivalenceSet{ terminal,nonTerminal };
	dividingSet(dfa, equivalenceSet);
	for (const SetStates& originalSet : equivalenceSet)
	{
		dfaMinimized.states.addState(originalSet);
	}

	for (const auto& [originalSet, id] : dfaMinimized.states.stateID_Bimap.left)
	{
		if (originalSet.find(dfa.start) != originalSet.end())
		{
			dfaMinimized.start = id;
			break;
		}
	}
	for (const auto& [originalSet, id] : dfaMinimized.states.stateID_Bimap.left)
	{
		if (setsHaveIntersect(originalSet, dfa.end))
		{
			dfaMinimized.end.insert(id);
		}
	}

	for (const auto& [inputName, inputID] : dfa.inputs.stateID_Bimap.left)
	{
		for (const auto& [originalSet, id] : dfaMinimized.states.stateID_Bimap.left)
		{
			StatesID sampleState = *originalSet.begin();
			auto targetIterator = dfa.transitionMapID.find(std::make_pair(sampleState, inputID));
			if (targetIterator != dfa.transitionMapID.end())
			{
				StatesID targetState = targetIterator->second;
				for (const auto& [targetSet, targetID] : dfaMinimized.states.stateID_Bimap.left)
				{
					if (targetSet.find(targetState) != targetSet.end())
					{
						dfaMinimized.transitionMapID[{id, inputID}] = targetID;
						break;
					}
				}
			}

		}
	}

}

void dividingSet(const DFA& dfa, std::set<SetStates>& equivalenceSet)
{
	bool changed;
	do
	{
	start:
		changed = false;
		std::set<SetStates> currentEquivalenceSet(equivalenceSet);
		for (const SetStates& originalSet : currentEquivalenceSet)
		{
			if (originalSet.size() == 1)
				continue;
			for (const auto& [inputName, inputID] : dfa.inputs.stateID_Bimap.left)
			{
				//dividing the set which is being iterating by it's elements' target value,
				// which set in two different equivalence sets
				std::map<SetStates, SetStates> splitMap;//std::map<targetEquivalenceSet,source>
				SetStates invalidStates;
				for (const StatesID state : originalSet)
				{
					auto targetIterator = dfa.transitionMapID.find(std::make_pair(state, inputID));
					if (targetIterator == dfa.transitionMapID.end())
					{
						invalidStates.insert(state);
						continue;
					}
					StatesID targetState = targetIterator->second;
					auto targetSetIter = std::find_if(
						equivalenceSet.begin(), equivalenceSet.end(),
						[targetState](const SetStates& set)
						{
							return set.find(targetState) != set.end();
						});

					if (targetSetIter != equivalenceSet.end())
					{
						splitMap[*targetSetIter].insert(state);
					}
				}
				if (!invalidStates.empty())
					splitMap[invalidStates] = invalidStates;
				if (splitMap.size() > 1)
				{
					changed = true;
					equivalenceSet.erase(originalSet);
					for (const auto& [_, part] : splitMap)
					{
						equivalenceSet.insert(part);
					}
					goto start;//若内层循环引发等价类改变，currentEquivalenceSet失真,重启循环
				}
			}
		}

	} while (changed);

}

