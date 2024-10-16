ExpressionObject::ExpressionObject(const boost::intrusive_ptr<ExpressionContext>& expCtx,
                                   vector<pair<string, intrusive_ptr<Expression>>>&& expressions)
    : Expression(expCtx), _expressions(std::move(expressions)) {}