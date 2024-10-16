Value ExpressionDateToParts::serialize(bool explain) const {
    return Value(
        Document{{"$dateToParts",
                  Document{{"date", _date->serialize(explain)},
                           {"timezone", _timeZone ? _timeZone->serialize(explain) : Value()},
                           {"iso8601", _iso8601 ? _iso8601->serialize(explain) : Value()}}}});
}