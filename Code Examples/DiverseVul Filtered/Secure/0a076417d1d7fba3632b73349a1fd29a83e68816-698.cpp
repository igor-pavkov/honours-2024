Value ExpressionReduce::serialize(bool explain) const {
    return Value(Document{{"$reduce",
                           Document{{"input", _input->serialize(explain)},
                                    {"initialValue", _initial->serialize(explain)},
                                    {"in", _in->serialize(explain)}}}});
}