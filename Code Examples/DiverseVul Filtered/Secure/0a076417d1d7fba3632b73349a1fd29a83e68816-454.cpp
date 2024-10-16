Value ExpressionDateToString::serialize(bool explain) const {
    return Value(
        Document{{"$dateToString",
                  Document{{"format", _format},
                           {"date", _date->serialize(explain)},
                           {"timezone", _timeZone ? _timeZone->serialize(explain) : Value()}}}});
}