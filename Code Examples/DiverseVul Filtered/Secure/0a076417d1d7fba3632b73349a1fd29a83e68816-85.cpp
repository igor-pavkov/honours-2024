Value ExpressionDateFromParts::evaluate(const Document& root) const {
    int hour, minute, second, millisecond;

    if (!evaluateNumberWithinRange(root, _hour.get(), "hour"_sd, 0, 0, 24, &hour) ||
        !evaluateNumberWithinRange(root, _minute.get(), "minute"_sd, 0, 0, 59, &minute) ||
        !evaluateNumberWithinRange(root, _second.get(), "second"_sd, 0, 0, 59, &second) ||
        !evaluateNumberWithinRange(
            root, _millisecond.get(), "millisecond"_sd, 0, 0, 999, &millisecond)) {
        return Value(BSONNULL);
    }

    auto timeZone = makeTimeZone(getExpressionContext()->timeZoneDatabase, root, _timeZone.get());

    if (!timeZone) {
        return Value(BSONNULL);
    }

    if (_year) {
        int year, month, day;

        if (!evaluateNumberWithinRange(root, _year.get(), "year"_sd, 1970, 0, 9999, &year) ||
            !evaluateNumberWithinRange(root, _month.get(), "month"_sd, 1, 1, 12, &month) ||
            !evaluateNumberWithinRange(root, _day.get(), "day"_sd, 1, 1, 31, &day)) {
            return Value(BSONNULL);
        }

        return Value(
            timeZone->createFromDateParts(year, month, day, hour, minute, second, millisecond));
    }

    if (_isoWeekYear) {
        int isoWeekYear, isoWeek, isoDayOfWeek;

        if (!evaluateNumberWithinRange(
                root, _isoWeekYear.get(), "isoWeekYear"_sd, 1970, 0, 9999, &isoWeekYear) ||
            !evaluateNumberWithinRange(root, _isoWeek.get(), "isoWeek"_sd, 1, 1, 53, &isoWeek) ||
            !evaluateNumberWithinRange(
                root, _isoDayOfWeek.get(), "isoDayOfWeek"_sd, 1, 1, 7, &isoDayOfWeek)) {
            return Value(BSONNULL);
        }

        return Value(timeZone->createFromIso8601DateParts(
            isoWeekYear, isoWeek, isoDayOfWeek, hour, minute, second, millisecond));
    }

    MONGO_UNREACHABLE;
}