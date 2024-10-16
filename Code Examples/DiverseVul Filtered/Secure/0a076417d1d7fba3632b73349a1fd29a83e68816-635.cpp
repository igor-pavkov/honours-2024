ExpressionConstant::ExpressionConstant(const boost::intrusive_ptr<ExpressionContext>& expCtx,
                                       const Value& value)
    : Expression(expCtx), _value(value) {}