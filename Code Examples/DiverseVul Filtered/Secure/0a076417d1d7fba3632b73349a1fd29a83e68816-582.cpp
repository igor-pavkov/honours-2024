intrusive_ptr<Expression> ExpressionMap::optimize() {
    // TODO handle when _input is constant
    _input = _input->optimize();
    _each = _each->optimize();
    return this;
}