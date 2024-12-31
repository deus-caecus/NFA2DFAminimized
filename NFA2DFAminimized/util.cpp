#include"util.h"


NFA::NFA(const std::map<std::pair<StatesName, InputName>, std::set<StatesName>>& transitionMapName,
	const StatesName& start, const std::set<StatesName>& end)
{

	for (const auto& item : transitionMapName)
	{
		const auto& [stateInputPair, targetStates] = item;
		const auto& [stateName, inputName] = stateInputPair;

		states.addState(stateName);
		inputs.addState(inputName);
	}
	for (const auto& item : transitionMapName)
	{
		const auto& [stateInputPair, targetStates] = item;
		const auto& [stateName, inputName] = stateInputPair;
		StatesID state = states.getIDByState(stateName);
		InputID  input = inputs.getIDByState(inputName);
		auto pair = std::make_pair(state, input);
		std::set<ID> targetIDset;
		for (const StatesName& state : targetStates)
		{// try to get the targetState when it doesn't exist 
			ID stateID = states.getIDByState(state);
			targetIDset.insert(stateID);
		}
		transitionMapID[pair] = targetIDset;
	}
	this->start = states.getIDByState(start);
	for (const StatesName& name : end)
	{
		this->end.insert(states.getIDByState(name));
	}
#if DEBUG
	std::cout << "states" << std::endl;
	for (const auto& [stateName, stateID] : this->states.stateID_Bimap.left)
		std::cout << stateName << ":" << stateID << std::endl;
	std::cout << "inputs" << std::endl;
	for (const auto& [inputName, inputID] : this->inputs.stateID_Bimap.left)
		std::cout << inputName << ":" << inputID << std::endl;
	std::cout << "transitionTable" << std::endl;
	for (const auto& [inputPairID, targetSet] : this->transitionMapID)
	{
		const auto& [sourceID, inputID] = inputPairID;
		std::cout << "Source ID: " << sourceID << ", Input ID: " << inputID << ' ';
		std::cout << "targetSet:";
		for (const auto& targetID : targetSet)
			std::cout << targetID << ",";
		std::cout << std::endl;
	}
#endif
}

std::set<ID> NFA::getNextStates(const std::set<ID>& currentStateSet, InputID inputID) const
{
	std::set<StatesID> startSet = getEpsilonClosure(currentStateSet);
	std::set<ID> nextStates;
	for (ID state : startSet)
	{
		auto pair = std::make_pair(state, inputID);
		const auto& it = this->transitionMapID.find(pair);
		if (it != transitionMapID.end())
		{
			nextStates.insert(it->second.begin(), it->second.end());
		}
	}

	return getEpsilonClosure(nextStates);
}

std::set<ID> NFA::getEpsilonClosure(const std::set<ID>& stateSet) const
{
	std::set<ID> closure(stateSet);
	std::set<ID> newStates;

	do
	{
		newStates.clear();
		for (const auto& state : closure)
		{
			auto it = transitionMapID.find({ state, inputs.getIDByState(Epsilon) });
			if (it == transitionMapID.end())
			{
				continue;
			}
			for (const auto& nextState : it->second)
			{
				if (closure.find(nextState) == closure.end())
				{
					newStates.insert(nextState);
				}
			}
		}
		closure.insert(newStates.begin(), newStates.end());
	} while (!newStates.empty());

	return closure;
}

void DFA::printTransitionMapName() const
{
	const int columnWidth = 20;
	std::cout << "start:" << this->start << " " << "end:" << this->end << std::endl;
	//table head
	const auto& inputView = inputs.stateID_Bimap.left;
	std::cout << std::left << std::setw(columnWidth) << "state\\input";
	for (const auto& inputPair : inputView)
	{
		std::cout << std::setw(columnWidth) << inputPair.first;
	}
	std::cout << std::endl;

	const auto& stateView = states.stateID_Bimap.left;
	for (const auto& [stateSet, stateID] : stateView)
	{
		std::ostringstream oss;
		oss << stateSet << ":" << stateID;

		std::cout << std::left << std::setw(columnWidth) << oss.str();
		for (const auto& [inputName, inputID] : inputView)
		{
			auto it = transitionMapID.find(std::make_pair(stateID, inputID));
			if (it != transitionMapID.end())
			{
				SetStates stateSet = this->states.getStateByID(it->second);
				std::ostringstream oss;
				oss << stateSet << ":" << it->second;
				std::string combinedString = oss.str();
				std::cout << std::setw(columnWidth) << combinedString;
			}
			else
			{
				std::cout << std::setw(columnWidth) << "-";
			}
		}
		std::cout << std::endl;
	}
}
