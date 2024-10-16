Value ExpressionSwitch::serialize(bool explain) const {
    std::vector<Value> serializedBranches;
    serializedBranches.reserve(_branches.size());

    for (auto&& branch : _branches) {
        serializedBranches.push_back(Value(Document{{"case", branch.first->serialize(explain)},
                                                    {"then", branch.second->serialize(explain)}}));
    }

    if (_default) {
        return Value(Document{{"$switch",
                               Document{{"branches", Value(serializedBranches)},
                                        {"default", _default->serialize(explain)}}}});
    }

    return Value(Document{{"$switch", Document{{"branches", Value(serializedBranches)}}}});
}