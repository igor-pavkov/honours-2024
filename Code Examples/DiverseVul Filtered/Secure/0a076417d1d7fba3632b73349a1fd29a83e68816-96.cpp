void ExpressionDateFromString::_doAddDependencies(DepsTracker* deps) const {
    _dateString->addDependencies(deps);
    if (_timeZone) {
        _timeZone->addDependencies(deps);
    }
}