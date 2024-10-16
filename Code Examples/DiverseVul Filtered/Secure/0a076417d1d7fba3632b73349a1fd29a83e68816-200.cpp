intrusive_ptr<Expression> ExpressionDateToParts::optimize() {
    _date = _date->optimize();
    if (_timeZone) {
        _timeZone = _timeZone->optimize();
    }
    if (_iso8601) {
        _iso8601 = _iso8601->optimize();
    }

    if (ExpressionConstant::allNullOrConstant({_date, _iso8601, _timeZone})) {
        // Everything is a constant, so we can turn into a constant.
        return ExpressionConstant::create(getExpressionContext(), evaluate(Document{}));
    }

    return this;
}