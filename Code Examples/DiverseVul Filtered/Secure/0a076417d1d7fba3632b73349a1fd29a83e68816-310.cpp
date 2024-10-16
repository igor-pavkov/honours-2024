intrusive_ptr<Expression> ExpressionDateToString::parse(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    BSONElement expr,
    const VariablesParseState& vps) {
    verify(str::equals(expr.fieldName(), "$dateToString"));

    uassert(18629, "$dateToString only supports an object as its argument", expr.type() == Object);

    BSONElement formatElem;
    BSONElement dateElem;
    BSONElement timeZoneElem;
    const BSONObj args = expr.embeddedObject();
    BSONForEach(arg, args) {
        if (str::equals(arg.fieldName(), "format")) {
            formatElem = arg;
        } else if (str::equals(arg.fieldName(), "date")) {
            dateElem = arg;
        } else if (str::equals(arg.fieldName(), "timezone")) {
            timeZoneElem = arg;
        } else {
            uasserted(18534,
                      str::stream() << "Unrecognized argument to $dateToString: "
                                    << arg.fieldName());
        }
    }

    uassert(18627, "Missing 'format' parameter to $dateToString", !formatElem.eoo());
    uassert(18628, "Missing 'date' parameter to $dateToString", !dateElem.eoo());

    uassert(18533,
            "The 'format' parameter to $dateToString must be a string literal",
            formatElem.type() == String);

    const string format = formatElem.str();

    TimeZone::validateFormat(format);

    return new ExpressionDateToString(expCtx,
                                      format,
                                      parseOperand(expCtx, dateElem, vps),
                                      timeZoneElem ? parseOperand(expCtx, timeZoneElem, vps)
                                                   : nullptr);
}