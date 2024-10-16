Value ExpressionDateFromString::evaluate(const Document& root) const {
    const Value dateString = _dateString->evaluate(root);

    auto timeZone = makeTimeZone(getExpressionContext()->timeZoneDatabase, root, _timeZone.get());

    if (!timeZone || dateString.nullish()) {
        return Value(BSONNULL);
    }

    uassert(40543,
            str::stream() << "$dateFromString requires that 'dateString' be a string, found: "
                          << typeName(dateString.getType())
                          << " with value "
                          << dateString.toString(),
            dateString.getType() == BSONType::String);
    const std::string& dateTimeString = dateString.getString();

    return Value(getExpressionContext()->timeZoneDatabase->fromString(dateTimeString, timeZone));
}