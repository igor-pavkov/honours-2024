Value ExpressionCoerceToBool::serialize(bool explain) const {
    // When not explaining, serialize to an $and expression. When parsed, the $and expression
    // will be optimized back into a ExpressionCoerceToBool.
    const char* name = explain ? "$coerceToBool" : "$and";
    return Value(DOC(name << DOC_ARRAY(pExpression->serialize(explain))));
}