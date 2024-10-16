ExpressionDateToString::ExpressionDateToString(
    const boost::intrusive_ptr<ExpressionContext>& expCtx,
    const string& format,
    intrusive_ptr<Expression> date,
    intrusive_ptr<Expression> timeZone)
    : Expression(expCtx), _format(format), _date(std::move(date)), _timeZone(std::move(timeZone)) {}