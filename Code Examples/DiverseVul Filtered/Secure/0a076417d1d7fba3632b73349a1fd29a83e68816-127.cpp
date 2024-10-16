ValueSet arrayToSet(const Value& val, const ValueComparator& valueComparator) {
    const vector<Value>& array = val.getArray();
    ValueSet valueSet = valueComparator.makeOrderedValueSet();
    valueSet.insert(array.begin(), array.end());
    return valueSet;
}