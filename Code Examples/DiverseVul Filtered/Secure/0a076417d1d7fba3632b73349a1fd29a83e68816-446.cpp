void ExpressionFieldPath::_doAddDependencies(DepsTracker* deps) const {
    if (_variable == Variables::kRootId) {  // includes CURRENT when it is equivalent to ROOT.
        if (_fieldPath.getPathLength() == 1) {
            deps->needWholeDocument = true;  // need full doc if just "$$ROOT"
        } else {
            deps->fields.insert(_fieldPath.tail().fullPath());
        }
    } else if (Variables::isUserDefinedVariable(_variable)) {
        deps->vars.insert(_variable);
    }
}