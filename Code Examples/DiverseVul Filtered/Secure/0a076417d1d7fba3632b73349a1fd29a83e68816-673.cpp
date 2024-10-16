intrusive_ptr<Expression> ExpressionDateFromString::optimize() {
    _dateString = _dateString->optimize();
    if (_timeZone) {
        _timeZone = _timeZone->optimize();
    }

    if (ExpressionConstant::allNullOrConstant({_dateString, _timeZone})) {
        // Everything is a constant, so we can turn into a constant.
        return ExpressionConstant::create(getExpressionContext(), evaluate(Document{}));
    }
    return this;
}