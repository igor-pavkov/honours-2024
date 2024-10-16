intrusive_ptr<Expression> ExpressionReduce::optimize() {
    _input = _input->optimize();
    _initial = _initial->optimize();
    _in = _in->optimize();
    return this;
}