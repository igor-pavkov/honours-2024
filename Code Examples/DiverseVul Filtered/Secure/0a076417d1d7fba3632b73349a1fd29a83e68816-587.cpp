boost::intrusive_ptr<Expression> ExpressionSwitch::optimize() {
    if (_default) {
        _default = _default->optimize();
    }

    std::transform(_branches.begin(),
                   _branches.end(),
                   _branches.begin(),
                   [](ExpressionPair branch) -> ExpressionPair {
                       return {branch.first->optimize(), branch.second->optimize()};
                   });

    return this;
}