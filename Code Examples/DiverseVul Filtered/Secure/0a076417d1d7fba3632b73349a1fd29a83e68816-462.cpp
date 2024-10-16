bool ExpressionDateFromParts::evaluateNumberWithinRange(const Document& root,
                                                        const Expression* field,
                                                        StringData fieldName,
                                                        int defaultValue,
                                                        int minValue,
                                                        int maxValue,
                                                        int* returnValue) const {
    if (!field) {
        *returnValue = defaultValue;
        return true;
    }

    auto fieldValue = field->evaluate(root);

    if (fieldValue.nullish()) {
        return false;
    }

    uassert(40515,
            str::stream() << "'" << fieldName << "' must evaluate to an integer, found "
                          << typeName(fieldValue.getType())
                          << " with value "
                          << fieldValue.toString(),
            fieldValue.integral());

    *returnValue = fieldValue.coerceToInt();

    uassert(40523,
            str::stream() << "'" << fieldName << "' must evaluate to an integer in the range "
                          << minValue
                          << " to "
                          << maxValue
                          << ", found "
                          << *returnValue,
            *returnValue >= minValue && *returnValue <= maxValue);

    return true;
}