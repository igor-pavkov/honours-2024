void assertInvalidFindProjection(const char* queryStr, const char* projStr, size_t errCode) {
    BSONObj query = fromjson(queryStr);
    BSONObj projObj = fromjson(projStr);
    QueryTestServiceContext serviceCtx;
    auto opCtx = serviceCtx.makeOperationContext();
    const boost::intrusive_ptr<ExpressionContext> expCtx(
        new ExpressionContext(opCtx.get(), std::unique_ptr<CollatorInterface>(nullptr), kTestNss));
    StatusWithMatchExpression statusWithMatcher =
        MatchExpressionParser::parse(query, std::move(expCtx));
    ASSERT_OK(statusWithMatcher.getStatus());
    std::unique_ptr<MatchExpression> queryMatchExpr = std::move(statusWithMatcher.getValue());
    ASSERT_THROWS_CODE(projection_ast::parse(expCtx,
                                             projObj,
                                             queryMatchExpr.get(),
                                             query,
                                             ProjectionPolicies::findProjectionPolicies()),
                       DBException,
                       errCode);
}