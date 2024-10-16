bool attemptToParseGenericExpression(ParseContext* parseCtx,
                                     const FieldPath& path,
                                     const BSONObj& subObj,
                                     ProjectionPathASTNode* parent) {
    if (!Expression::isExpressionName(subObj.firstElementFieldNameStringData())) {
        return false;
    }

    // It must be an expression.
    verifyComputedFieldsAllowed(parseCtx->policies);

    const bool isMeta = subObj.firstElementFieldNameStringData() == "$meta";
    uassert(31252,
            "Cannot use expression other than $meta in exclusion projection",
            !parseCtx->type || *parseCtx->type == ProjectType::kInclusion || isMeta);

    if (!isMeta) {
        parseCtx->type = ProjectType::kInclusion;
    }

    auto expr = Expression::parseExpression(
        parseCtx->expCtx, subObj, parseCtx->expCtx->variablesParseState);
    addNodeAtPath(parent, path, std::make_unique<ExpressionASTNode>(expr));
    parseCtx->hasMeta = parseCtx->hasMeta || isMeta;
    return true;
}