ExpressionFilter::ExpressionFilter(const boost::intrusive_ptr<ExpressionContext>& expCtx,
                                   string varName,
                                   Variables::Id varId,
                                   intrusive_ptr<Expression> input,
                                   intrusive_ptr<Expression> filter)
    : Expression(expCtx),
      _varName(std::move(varName)),
      _varId(varId),
      _input(std::move(input)),
      _filter(std::move(filter)) {}