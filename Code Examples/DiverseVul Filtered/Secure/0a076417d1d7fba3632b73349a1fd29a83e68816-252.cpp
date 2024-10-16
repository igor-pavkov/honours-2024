ExpressionDateFromString::ExpressionDateFromString(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    intrusive_ptr<Expression> dateString,
    intrusive_ptr<Expression> timeZone)
    : Expression(expCtx), _dateString(std::move(dateString)), _timeZone(std::move(timeZone)) {}