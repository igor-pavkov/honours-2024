void ExpressionObject::_doAddDependencies(DepsTracker* deps) const {
    for (auto&& pair : _expressions) {
        pair.second->addDependencies(deps);
    }
}