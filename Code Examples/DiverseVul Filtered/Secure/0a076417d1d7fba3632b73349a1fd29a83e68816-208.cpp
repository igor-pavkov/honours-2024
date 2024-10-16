intrusive_ptr<Expression> ExpressionFieldPath::optimize() {
    if (_variable == Variables::kRemoveId) {
        // The REMOVE system variable optimizes to a constant missing value.
        return ExpressionConstant::create(getExpressionContext(), Value());
    }

    if (getExpressionContext()->variables.hasConstantValue(_variable)) {
        return ExpressionConstant::create(getExpressionContext(), evaluate(Document()));
    }

    return intrusive_ptr<Expression>(this);
}