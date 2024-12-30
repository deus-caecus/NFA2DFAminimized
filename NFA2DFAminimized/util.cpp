#include"util.h"


NFA::NFA(const std::map<std::pair<StatesName, InputName>, std::set<StatesName>>& transitionMapName,
    const StatesName& start, const std::set<StatesName>& end) {

    for (const auto& item : transitionMapName) {
        const auto& [stateInputPair, targetStates] = item;
        const auto& [stateName, inputName] = stateInputPair;

        ID stateID = states.addState(stateName);
        ID inputID = inputs.addState(inputName);

        auto pair = std::make_pair(stateID, inputID);
        std::set<ID> idset;
        for (const StatesName& state : targetStates) {
            stateID = states.getIDByState(state);
            idset.insert(stateID);
        }
        transitionMapID[pair] = idset;
    }
    this->start = states.getIDByState(start);
    for (const StatesName& name : end) {
        this->end.insert(states.getIDByState(name));
    }
}

std::set<ID> NFA::getNextStates(const std::set<ID>& currentStateSet, InputID inputID) const {
    std::set<StatesID> startSet = getEpsilonClosure(currentStateSet);
    std::set<ID> nextStates;
    for (ID state : startSet) {
        auto pair = std::make_pair(state, inputID);
        const auto& it = this->transitionMapID.find(pair);
        if (it != transitionMapID.end()) {
            nextStates.insert(it->second.begin(), it->second.end());
        }
    }

    return getEpsilonClosure(nextStates);
}

std::set<ID> NFA::getEpsilonClosure(const std::set<ID>& stateSet) const {
    std::set<ID> closure(stateSet);
    std::set<ID> newStates;

    do {
        newStates.clear();
        for (const auto& state : closure) {
            if (closure.size() != stateSet.size()) {
                auto it = transitionMapID.find({ state, inputs.getIDByState(Epsilon) });
                if (it != transitionMapID.end()) {
                    for (const auto& nextState : it->second) {
                        if (closure.find(nextState) == closure.end()) {
                            newStates.insert(nextState);
                        }
                    }
                }
            }
        }

        closure.insert(newStates.begin(), newStates.end());
    } while (!newStates.empty());

    return closure;
}

void DFA::printTransitionMapName() const {
    const int columnWidth = 20;
    const auto& inputView = inputs.stateID_Bimap.left;
    std::cout << std::left << std::setw(columnWidth) << "state\\input";
    for (const auto& inputPair : inputView) {
        std::cout << std::setw(columnWidth) << inputPair.first;
    }
    std::cout << std::endl;

    auto stateView = states.stateID_Bimap.left;
    for (const auto& [state, stateID] : stateView) {
        std::cout << std::left << std::setw(columnWidth) << stateID;
        for (const auto& inputPair : inputView) {
            auto it = transitionMapID.find(std::make_pair(stateID, inputPair.second));
            if (it != transitionMapID.end()) {
                StatesID targetID = it->second;
                if (targetID) {
                    std::cout << std::setw(columnWidth) << targetID;
                }
                else {
                    std::cout << std::setw(columnWidth) << "-";
                }
            }
            else {
                std::cout << std::setw(columnWidth) << "-";
            }
        }
    }
    std::cout << std::endl;
}
