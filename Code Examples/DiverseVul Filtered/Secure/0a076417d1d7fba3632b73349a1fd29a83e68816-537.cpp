intrusive_ptr<Expression> ExpressionDateFromString::parse(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    BSONElement expr,
    const VariablesParseState& vps) {

    uassert(40540,
            str::stream() << "$dateFromString only supports an object as an argument, found: "
                          << typeName(expr.type()),
            expr.type() == BSONType::Object);

    BSONElement dateStringElem;
    BSONElement timeZoneElem;

    const BSONObj args = expr.embeddedObject();
    for (auto&& arg : args) {
        auto field = arg.fieldNameStringData();

        if (field == "dateString"_sd) {
            dateStringElem = arg;
        } else if (field == "timezone"_sd) {
            timeZoneElem = arg;
        } else {
            uasserted(40541,
                      str::stream() << "Unrecognized argument to $dateFromString: "
                                    << arg.fieldName());
        }
    }

    uassert(40542, "Missing 'dateString' parameter to $dateFromString", dateStringElem);

    return new ExpressionDateFromString(expCtx,
                                        parseOperand(expCtx, dateStringElem, vps),
                                        timeZoneElem ? parseOperand(expCtx, timeZoneElem, vps)
                                                     : nullptr);
}