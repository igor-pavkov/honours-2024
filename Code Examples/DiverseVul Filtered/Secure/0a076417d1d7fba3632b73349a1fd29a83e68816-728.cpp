void ExpressionDateFromParts::_doAddDependencies(DepsTracker* deps) const {
    if (_year) {
        _year->addDependencies(deps);
    }
    if (_month) {
        _month->addDependencies(deps);
    }
    if (_day) {
        _day->addDependencies(deps);
    }
    if (_hour) {
        _hour->addDependencies(deps);
    }
    if (_minute) {
        _minute->addDependencies(deps);
    }
    if (_second) {
        _second->addDependencies(deps);
    }
    if (_millisecond) {
        _millisecond->addDependencies(deps);
    }
    if (_isoWeekYear) {
        _isoWeekYear->addDependencies(deps);
    }
    if (_isoWeek) {
        _isoWeek->addDependencies(deps);
    }
    if (_isoDayOfWeek) {
        _isoDayOfWeek->addDependencies(deps);
    }
    if (_timeZone) {
        _timeZone->addDependencies(deps);
    }
}