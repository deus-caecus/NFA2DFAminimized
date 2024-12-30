#pragma once
#include <iostream>
#include <string>
#include <set>
#include <map>
#include <boost/bimap.hpp>
#include <optional>
#include <iomanip>

#define Epsilon "ε"

using ID = int;
using StatesID = int;
using InputID = int;
using Name = std::string;
using StatesName = std::string;
using InputName = std::string;
using SetStates = std::set<StatesID>; // 用于表示子集法中的集合状态

// 判断集合相交
template <typename T>
bool inline setsHaveIntersect(const std::set<T>& set1, const std::set<T>& set2) {
    return std::any_of(
        set1.begin(), set1.end(),
        [&set2](const T& elem) { return set2.find(elem) != set2.end(); }
    );
}

template <typename StateType>
class MappingCollection {
public:
    ID autoID = 0;
    boost::bimap<StateType, ID> stateID_Bimap; // 每个state对应于唯一ID
    std::set<ID> usedID;

    MappingCollection() = default;

    ID addState(const StateType& state, std::optional<ID> id = std::nullopt) {
        ID insertID;

        if (id.has_value()) {
            insertID = id.value();
            if (usedID.find(insertID) != usedID.end()) {
                insertID = getNextAutoID();
            }
        }
        else {
            insertID = getNextAutoID();
        }

        auto result = stateID_Bimap.insert(boost::bimap<StateType, ID>::value_type(state, insertID));

        if (result.second) {
            usedID.insert(insertID);
        }
        else {
            return result.first->right;
        }
        return insertID;
    }
    ID getIDByState(const StateType& state) const {
        auto it = stateID_Bimap.left.find(state);
        if (it != stateID_Bimap.left.end()) {
            return it->second;
        }
        throw std::runtime_error("Name not found in mapping.");
    }
    StateType getStateByID(ID id) const {
        auto it = stateID_Bimap.right.find(id);
        if (it != stateID_Bimap.right.end()) {
            return it->second;
        }
        throw std::runtime_error("ID not found in mapping.");
    }

private:
    ID getNextAutoID() {
        while (usedID.find(autoID) != usedID.end()) {
            autoID++;
        }
        return autoID;
    }
};

class DFA;

class NFA {
public:
    MappingCollection<Name> states;
    MappingCollection<Name> inputs; // Name mapping to ID
    std::map<std::pair<StatesID, InputID>, std::set<StatesID>> transitionMapID;
    StatesID start;
    std::set<StatesID> end;

    NFA() = default;
    NFA(const std::map<std::pair<StatesName, InputName>, std::set<StatesName>>& transitionMapName,
        const StatesName& start, const std::set<StatesName>& end);

    std::set<ID> getNextStates(const std::set<ID>& currentStateSet, InputID inputID) const;
    std::set<ID> getEpsilonClosure(const std::set<ID>& stateSet) const;

    friend void NFA2DFA(const NFA& nfa, DFA& dfa);
};

class DFA {
public:
    MappingCollection<SetStates> states;
    MappingCollection<Name> nameID; // std::set<StatesID> or StatesName mapping to ID
    MappingCollection<Name> inputs; // Name mapping to ID
    std::map<std::pair<StatesID, InputID>, StatesID> transitionMapID;
    StatesID start;
    std::set<StatesID> end;

    DFA() = default;
    void printTransitionMapName() const;
};


