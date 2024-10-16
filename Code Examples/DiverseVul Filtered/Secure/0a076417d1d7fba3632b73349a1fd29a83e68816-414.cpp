Value ExpressionDateToParts::evaluate(const Document& root) const {
    const Value date = _date->evaluate(root);

    auto timeZone = makeTimeZone(getExpressionContext()->timeZoneDatabase, root, _timeZone.get());
    if (!timeZone) {
        return Value(BSONNULL);
    }

    auto iso8601 = evaluateIso8601Flag(root);
    if (!iso8601) {
        return Value(BSONNULL);
    }

    if (date.nullish()) {
        return Value(BSONNULL);
    }

    auto dateValue = date.coerceToDate();

    if (*iso8601) {
        auto parts = timeZone->dateIso8601Parts(dateValue);
        return Value(Document{{"isoWeekYear", parts.year},
                              {"isoWeek", parts.weekOfYear},
                              {"isoDayOfWeek", parts.dayOfWeek},
                              {"hour", parts.hour},
                              {"minute", parts.minute},
                              {"second", parts.second},
                              {"millisecond", parts.millisecond}});
    } else {
        auto parts = timeZone->dateParts(dateValue);
        return Value(Document{{"year", parts.year},
                              {"month", parts.month},
                              {"day", parts.dayOfMonth},
                              {"hour", parts.hour},
                              {"minute", parts.minute},
                              {"second", parts.second},
                              {"millisecond", parts.millisecond}});
    }
}