projection_ast::Projection createProjection(const BSONObj& query,
                                            const BSONObj& projObj,
                                            ProjectionPolicies policies = {}) {
    QueryTestServiceContext serviceCtx;
    auto opCtx = serviceCtx.makeOperationContext();
    const boost::intrusive_ptr<ExpressionContext> expCtx(
        new ExpressionContext(opCtx.get(), std::unique_ptr<CollatorInterface>(nullptr), kTestNss));
    StatusWithMatchExpression statusWithMatcher =
        MatchExpressionParser::parse(query, std::move(expCtx));
    ASSERT_OK(statusWithMatcher.getStatus());
    std::unique_ptr<MatchExpression> queryMatchExpr = std::move(statusWithMatcher.getValue());
    projection_ast::Projection res =
        projection_ast::parse(expCtx, projObj, queryMatchExpr.get(), query, policies);

    return res;
}