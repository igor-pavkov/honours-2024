boost::optional<TimeZone> makeTimeZone(const TimeZoneDatabase* tzdb,
                                       const Document& root,
                                       const Expression* timeZone) {
    invariant(tzdb);

    if (!timeZone) {
        return mongo::TimeZoneDatabase::utcZone();
    }

    auto timeZoneId = timeZone->evaluate(root);

    if (timeZoneId.nullish()) {
        return boost::none;
    }

    uassert(40517,
            str::stream() << "timezone must evaluate to a string, found "
                          << typeName(timeZoneId.getType()),
            timeZoneId.getType() == BSONType::String);

    return tzdb->getTimeZone(timeZoneId.getString());
}