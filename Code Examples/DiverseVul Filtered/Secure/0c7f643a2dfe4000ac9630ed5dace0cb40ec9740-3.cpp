void parseLiteral(ParseContext* ctx, BSONElement elem, ProjectionPathASTNode* parent) {
    verifyComputedFieldsAllowed(ctx->policies);

    auto expr = Expression::parseOperand(ctx->expCtx, elem, ctx->expCtx->variablesParseState);

    FieldPath pathFromParent(elem.fieldNameStringData());
    addNodeAtPath(parent, pathFromParent, std::make_unique<ExpressionASTNode>(expr));

    uassert(31310,
            str::stream() << "Cannot use an expression " << elem << " in an exclusion projection",
            !ctx->type || *ctx->type == ProjectType::kInclusion);
    ctx->type = ProjectType::kInclusion;
}