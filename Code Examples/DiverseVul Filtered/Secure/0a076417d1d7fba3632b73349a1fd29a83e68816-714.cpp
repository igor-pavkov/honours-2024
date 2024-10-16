Value ExpressionFieldPath::serialize(bool explain) const {
    if (_fieldPath.getFieldName(0) == "CURRENT" && _fieldPath.getPathLength() > 1) {
        // use short form for "$$CURRENT.foo" but not just "$$CURRENT"
        return Value("$" + _fieldPath.tail().fullPath());
    } else {
        return Value("$$" + _fieldPath.fullPath());
    }
}