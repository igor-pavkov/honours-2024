intrusive_ptr<Expression> ExpressionDateFromParts::optimize() {
    if (_year) {
        _year = _year->optimize();
    }
    if (_month) {
        _month = _month->optimize();
    }
    if (_day) {
        _day = _day->optimize();
    }
    if (_hour) {
        _hour = _hour->optimize();
    }
    if (_minute) {
        _minute = _minute->optimize();
    }
    if (_second) {
        _second = _second->optimize();
    }
    if (_millisecond) {
        _millisecond = _millisecond->optimize();
    }
    if (_isoWeekYear) {
        _isoWeekYear = _isoWeekYear->optimize();
    }
    if (_isoWeek) {
        _isoWeek = _isoWeek->optimize();
    }
    if (_isoDayOfWeek) {
        _isoDayOfWeek = _isoDayOfWeek->optimize();
    }
    if (_timeZone) {
        _timeZone = _timeZone->optimize();
    }

    if (ExpressionConstant::allNullOrConstant({_year,
                                               _month,
                                               _day,
                                               _hour,
                                               _minute,
                                               _second,
                                               _millisecond,
                                               _isoWeekYear,
                                               _isoWeek,
                                               _isoDayOfWeek,
                                               _timeZone})) {
        // Everything is a constant, so we can turn into a constant.
        return ExpressionConstant::create(getExpressionContext(), evaluate(Document{}));
    }

    return this;
}