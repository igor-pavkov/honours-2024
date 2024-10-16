Value ExpressionDateFromString::serialize(bool explain) const {
    return Value(
        Document{{"$dateFromString",
                  Document{{"dateString", _dateString->serialize(explain)},
                           {"timezone", _timeZone ? _timeZone->serialize(explain) : Value()}}}});
}