Value ExpressionDateFromParts::serialize(bool explain) const {
    return Value(Document{
        {"$dateFromParts",
         Document{{"year", _year ? _year->serialize(explain) : Value()},
                  {"month", _month ? _month->serialize(explain) : Value()},
                  {"day", _day ? _day->serialize(explain) : Value()},
                  {"hour", _hour ? _hour->serialize(explain) : Value()},
                  {"minute", _minute ? _minute->serialize(explain) : Value()},
                  {"second", _second ? _second->serialize(explain) : Value()},
                  {"millisecond", _millisecond ? _millisecond->serialize(explain) : Value()},
                  {"isoWeekYear", _isoWeekYear ? _isoWeekYear->serialize(explain) : Value()},
                  {"isoWeek", _isoWeek ? _isoWeek->serialize(explain) : Value()},
                  {"isoDayOfWeek", _isoDayOfWeek ? _isoDayOfWeek->serialize(explain) : Value()},
                  {"timezone", _timeZone ? _timeZone->serialize(explain) : Value()}}}});
}