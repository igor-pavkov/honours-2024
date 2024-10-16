void ExpressionZip::_doAddDependencies(DepsTracker* deps) const {
    std::for_each(
        _inputs.begin(), _inputs.end(), [&deps](intrusive_ptr<Expression> inputExpression) -> void {
            inputExpression->addDependencies(deps);
        });
    std::for_each(_defaults.begin(),
                  _defaults.end(),
                  [&deps](intrusive_ptr<Expression> defaultExpression) -> void {
                      defaultExpression->addDependencies(deps);
                  });
}