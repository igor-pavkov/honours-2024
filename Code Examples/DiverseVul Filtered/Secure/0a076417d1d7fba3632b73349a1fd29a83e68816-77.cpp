Value ExpressionMap::serialize(bool explain) const {
    return Value(DOC("$map" << DOC("input" << _input->serialize(explain) << "as" << _varName << "in"
                                           << _each->serialize(explain))));
}