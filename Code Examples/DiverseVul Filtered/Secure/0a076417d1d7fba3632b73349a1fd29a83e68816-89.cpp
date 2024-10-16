Value ExpressionFieldPath::evaluate(const Document& root) const {
    auto& vars = getExpressionContext()->variables;
    if (_fieldPath.getPathLength() == 1)  // get the whole variable
        return vars.getValue(_variable, root);

    if (_variable == Variables::kRootId) {
        // ROOT is always a document so use optimized code path
        return evaluatePath(1, root);
    }

    Value var = vars.getValue(_variable, root);
    switch (var.getType()) {
        case Object:
            return evaluatePath(1, var.getDocument());
        case Array:
            return evaluatePathArray(1, var);
        default:
            return Value();
    }
}