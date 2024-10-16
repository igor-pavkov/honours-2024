ExpressionLet::ExpressionLet(const boost::intrusive_ptr<ExpressionContext>& expCtx,
                             const VariableMap& vars,
                             intrusive_ptr<Expression> subExpression)
    : Expression(expCtx), _variables(vars), _subExpression(subExpression) {}