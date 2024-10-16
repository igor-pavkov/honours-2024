Value ExpressionDateToString::evaluate(const Document& root) const {
    const Value date = _date->evaluate(root);

    auto timeZone = makeTimeZone(getExpressionContext()->timeZoneDatabase, root, _timeZone.get());
    if (!timeZone) {
        return Value(BSONNULL);
    }

    if (date.nullish()) {
        return Value(BSONNULL);
    }

    return Value(timeZone->formatDate(_format, date.coerceToDate()));
}