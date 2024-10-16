void ExpressionDateToParts::_doAddDependencies(DepsTracker* deps) const {
    _date->addDependencies(deps);
    if (_timeZone) {
        _timeZone->addDependencies(deps);
    }
    if (_iso8601) {
        _iso8601->addDependencies(deps);
    }
}