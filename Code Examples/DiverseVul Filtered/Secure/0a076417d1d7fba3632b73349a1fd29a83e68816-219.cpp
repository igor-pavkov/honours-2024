boost::optional<int> ExpressionDateToParts::evaluateIso8601Flag(const Document& root) const {
    if (!_iso8601) {
        return false;
    }

    auto iso8601Output = _iso8601->evaluate(root);

    if (iso8601Output.nullish()) {
        return boost::none;
    }

    uassert(40521,
            str::stream() << "iso8601 must evaluate to a bool, found "
                          << typeName(iso8601Output.getType()),
            iso8601Output.getType() == BSONType::Bool);

    return iso8601Output.getBool();
}