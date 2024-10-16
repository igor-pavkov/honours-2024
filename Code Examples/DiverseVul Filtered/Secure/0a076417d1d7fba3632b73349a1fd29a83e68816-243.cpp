intrusive_ptr<Expression> ExpressionFilter::optimize() {
    // TODO handle when _input is constant.
    _input = _input->optimize();
    _filter = _filter->optimize();
    return this;
}