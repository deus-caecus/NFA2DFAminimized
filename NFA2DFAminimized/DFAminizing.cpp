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
		if (setsHaveIntersect(originalSet, dfaMinimized.end))
		{
			dfaMinimized.end.insert(id);
			break;
		}
	}

	for (const auto& [inputName, inputID] : dfa.inputs.stateID_Bimap.left)
	{
		for (const auto& [originalSet, id] : dfaMinimized.states.stateID_Bimap.left)
		{
			for (StatesID state : originalSet)
			{
				auto targetIterator = dfa.transitionMapID.find(std::make_pair(state, inputID));
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

}

void dividingSet(const DFA& dfa,std::set<SetStates>& equivalenceSet)
{
	bool changed = false;
	std::set<SetStates> newEquivalenceSet;
	std::set<SetStates> currentEquivalenceSet(equivalenceSet);
	for (const SetStates& originalSet : currentEquivalenceSet)
	{
		std::set<SetStates> splitSet;
		for (const auto& [inputName, inputID] : dfa.inputs.stateID_Bimap.left)
		{
			std::multimap<SetStates, StatesID> divisionByTargetSet;

			//dividing the set which is being iterating by it's elements' target value,
			// which set in different equivalence sets
			for (const StatesID state : originalSet)
			{
				auto targetIterator= dfa.transitionMapID.find(std::make_pair(state,inputID));
				if (targetIterator != dfa.transitionMapID.end())
				{
					//find the targeting equivalenceSet
					StatesID targetState = targetIterator->second;
					for (const SetStates& set : currentEquivalenceSet)
					{
						if (set.find(targetState) != set.end())
						{
							divisionByTargetSet.insert(std::make_pair(set, state));
							break;
						}
					}
				}
			}

			for (const auto& targetSourceStatePair : divisionByTargetSet)
			{
				std::set<StatesID> newSet;

				auto range = divisionByTargetSet.equal_range(targetSourceStatePair.first);
				
				for (auto& it = range.first; it != range.second; ++it)
				{
					newSet.insert(it->second);
				}
				if (!newSet.empty())
				{
					splitSet.insert(newSet);
				}
			}

			if(splitSet.size()>1)
			{
				changed = true;
				equivalenceSet.insert(splitSet.begin(), splitSet.end());
				equivalenceSet.erase(originalSet);
			}
		}
	}
	if (changed)
		dividingSet(dfa, equivalenceSet);
}

