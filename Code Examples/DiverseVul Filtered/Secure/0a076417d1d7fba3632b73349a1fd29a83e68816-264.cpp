intrusive_ptr<Expression> ExpressionDateToString::optimize() {
    _date = _date->optimize();
    if (_timeZone) {
        _timeZone = _timeZone->optimize();
    }

    if (ExpressionConstant::allNullOrConstant({_date, _timeZone})) {
        // Everything is a constant, so we can turn into a constant.
        return ExpressionConstant::create(getExpressionContext(), evaluate(Document{}));
    }

    return this;
}