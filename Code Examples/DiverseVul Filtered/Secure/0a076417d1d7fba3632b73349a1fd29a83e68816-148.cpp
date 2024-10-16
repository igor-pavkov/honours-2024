Value ExpressionFilter::serialize(bool explain) const {
    return Value(
        DOC("$filter" << DOC("input" << _input->serialize(explain) << "as" << _varName << "cond"
                                     << _filter->serialize(explain))));
}