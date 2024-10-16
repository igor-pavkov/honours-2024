ExpressionFieldPath::ExpressionFieldPath(const boost::intrusive_ptr<ExpressionContext>& expCtx,
                                         const string& theFieldPath,
                                         Variables::Id variable)
    : Expression(expCtx), _fieldPath(theFieldPath), _variable(variable) {}