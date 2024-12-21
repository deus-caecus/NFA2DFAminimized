#pragma once
#include "debug_macros.h"
#include<string>
#include<set>
#include<map>
#include<boost/bimap.hpp>
#include<optional>
using ID = int;
using StatesID = int;
using InputID = int;
using Name = std::string;
using StatesName = std::string;
using InputName = std::string;


class MappingCollection
{
protected:
	ID autoID = 0;
	boost::bimap<Name, ID> nameID_Bimap;
	std::set<ID> usedID;
public:
	MappingCollection() = default;
	//MappingCollection(const std::set<Name>& nameSet)
	//{
	//	for (const auto& Name : nameSet)
	//	{
	//		addName(Name);
	//	}
	//}

	ID addName( const Name& name,std::optional<ID> id = std::nullopt )
	{
		ID insertID ;

		if (id.has_value()) 
		{
			insertID = id.value();
			if (usedID.find(insertID) != usedID.end()) 
			{
				DEBUG("Provided ID is already in use,available autoID will be used");
				insertID = getNextAutoID();
			}
		}
		else
		{
			insertID = getNextAutoID();
		}
		auto result =nameID_Bimap.insert(boost::bimap<Name, ID>::value_type(name, insertID));

		if (result.second)
		{
			DEBUG("Inserted element: " << result.first->left << " -> " << result.first->right << "\n");
		}
		else
		{
			DEBUG( "Element already exists.\n");
		}
		return insertID;
	}

	ID getIDByName(const Name& name) const {
		auto it = nameID_Bimap.left.find(name);
		if (it != nameID_Bimap.left.end()) {
			DEBUG("Found ID for name: " << name << " -> " << it->second << "\n");
			return it->second;
		}
		throw std::runtime_error("Name not found in mapping.");
	}

	Name getNameByID(ID id) const {
		auto it = nameID_Bimap.right.find(id);
		if (it != nameID_Bimap.right.end()) {
			DEBUG("Found name for ID: " << id << " -> " << it->second << "\n");
			return it->second;
		}
		throw std::runtime_error("ID not found in mapping.");
	}
private:
	ID getNextAutoID() {
		while (usedID.find(autoID) != usedID.end()) {
			autoID++;
		}
		return autoID++;
	}
};

class StatesMappingCollection :public MappingCollection{};

class InputMappingCollection:public MappingCollection{};

//class TransitionFunction
//{
//private:
//	std::map<std::pair<StatesID, InputID>, std::set<StatesID>> transitionMap;
//public:
//	TransitionFunction() {};
//
//	void addMap(StatesID startState,InputID input,std::set<StatesID> endStates)
//	{
//		auto key = std::make_pair(startState, input);
//		transitionMap[key] = endStates;
//	}
//};

class FSM //Finite State Machine
{
private:
	StatesMappingCollection states;
	InputMappingCollection inputs;
	std::map<std::pair<StatesID, InputID>, std::set<StatesID>> transitionMapID;
	StatesID start;
	std::set<StatesID> end;
public:
	FSM(const std::map<std::pair<StatesName, InputName>, std::set<StatesName>>& transitionMapName,
		const StatesName& start,std::set<StatesName> end)
	{
		for (const auto& item : transitionMapName)
		{
			const auto& [stateInputPair, targetStates] = item;
			const auto& [stateName, inputName] = stateInputPair;

			ID stateID = states.addName(stateName);
			ID inputID = inputs.addName(inputName);

			auto pair = std::make_pair(stateID, inputID);
			std::set<ID> idset;
			for (const auto& state : targetStates)
			{
				stateID = states.getIDByName(state);
				idset.insert(stateID);
			}
			transitionMapID[pair] = idset;
		}
		
	}


};

