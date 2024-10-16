TEST(ParseObject, ShouldAcceptEmptyObject) {
    auto resultExpression = parseObject(BSONObj());

    // Should return an empty ExpressionObject.
    auto resultObject = dynamic_cast<ExpressionObject*>(resultExpression.get());
    ASSERT_TRUE(resultObject);

    ASSERT_EQ(resultObject->getChildExpressions().size(), 0UL);
}