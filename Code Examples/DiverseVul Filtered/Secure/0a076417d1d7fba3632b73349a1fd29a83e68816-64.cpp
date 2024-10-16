Value setIsSubsetHelper(const vector<Value>& lhs, const ValueSet& rhs) {
    // do not shortcircuit when lhs.size() > rhs.size()
    // because lhs can have redundant entries
    for (vector<Value>::const_iterator it = lhs.begin(); it != lhs.end(); ++it) {
        if (!rhs.count(*it)) {
            return Value(false);
        }
    }
    return Value(true);
}