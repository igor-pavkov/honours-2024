ExpressionDateToParts::ExpressionDateToParts(const boost::intrusive_ptr<ExpressionContext>& expCtx,
                                             intrusive_ptr<Expression> date,
                                             intrusive_ptr<Expression> timeZone,
                                             intrusive_ptr<Expression> iso8601)
    : Expression(expCtx),
      _date(std::move(date)),
      _timeZone(std::move(timeZone)),
      _iso8601(std::move(iso8601)) {}