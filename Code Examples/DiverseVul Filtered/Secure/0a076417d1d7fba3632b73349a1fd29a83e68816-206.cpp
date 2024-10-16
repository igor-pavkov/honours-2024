ExpressionCoerceToBool::ExpressionCoerceToBool(const intrusive_ptr<ExpressionContext>& expCtx,
                                               const intrusive_ptr<Expression>& pTheExpression)
    : Expression(expCtx), pExpression(pTheExpression) {}