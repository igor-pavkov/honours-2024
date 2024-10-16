void ExpressionSwitch::_doAddDependencies(DepsTracker* deps) const {
    for (auto&& branch : _branches) {
        branch.first->addDependencies(deps);
        branch.second->addDependencies(deps);
    }

    if (_default) {
        _default->addDependencies(deps);
    }
}