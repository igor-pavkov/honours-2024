Value ExpressionAdd::evaluate(const Document& root) const {
    // We'll try to return the narrowest possible result value while avoiding overflow, loss
    // of precision due to intermediate rounding or implicit use of decimal types. To do that,
    // compute a compensated sum for non-decimal values and a separate decimal sum for decimal
    // values, and track the current narrowest type.
    DoubleDoubleSummation nonDecimalTotal;
    Decimal128 decimalTotal;
    BSONType totalType = NumberInt;
    bool haveDate = false;

    const size_t n = vpOperand.size();
    for (size_t i = 0; i < n; ++i) {
        Value val = vpOperand[i]->evaluate(root);

        switch (val.getType()) {
            case NumberDecimal:
                decimalTotal = decimalTotal.add(val.getDecimal());
                totalType = NumberDecimal;
                break;
            case NumberDouble:
                nonDecimalTotal.addDouble(val.getDouble());
                if (totalType != NumberDecimal)
                    totalType = NumberDouble;
                break;
            case NumberLong:
                nonDecimalTotal.addLong(val.getLong());
                if (totalType == NumberInt)
                    totalType = NumberLong;
                break;
            case NumberInt:
                nonDecimalTotal.addDouble(val.getInt());
                break;
            case Date:
                uassert(16612, "only one date allowed in an $add expression", !haveDate);
                haveDate = true;
                nonDecimalTotal.addLong(val.getDate().toMillisSinceEpoch());
                break;
            default:
                uassert(16554,
                        str::stream() << "$add only supports numeric or date types, not "
                                      << typeName(val.getType()),
                        val.nullish());
                return Value(BSONNULL);
        }
    }

    if (haveDate) {
        int64_t longTotal;
        if (totalType == NumberDecimal) {
            longTotal = decimalTotal.add(nonDecimalTotal.getDecimal()).toLong();
        } else {
            uassert(ErrorCodes::Overflow, "date overflow in $add", nonDecimalTotal.fitsLong());
            longTotal = nonDecimalTotal.getLong();
        }
        return Value(Date_t::fromMillisSinceEpoch(longTotal));
    }
    switch (totalType) {
        case NumberDecimal:
            return Value(decimalTotal.add(nonDecimalTotal.getDecimal()));
        case NumberLong:
            dassert(nonDecimalTotal.isInteger());
            if (nonDecimalTotal.fitsLong())
                return Value(nonDecimalTotal.getLong());
        // Fallthrough.
        case NumberInt:
            if (nonDecimalTotal.fitsLong())
                return Value::createIntOrLong(nonDecimalTotal.getLong());
        // Fallthrough.
        case NumberDouble:
            return Value(nonDecimalTotal.getDouble());
        default:
            massert(16417, "$add resulted in a non-numeric type", false);
    }
}