TEST_F(ExpressionNaryTest, ValidateFieldPathExpressionDependency) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    _notAssociativeNorCommutative->addOperand(ExpressionFieldPath::create(expCtx, "ab.c"));
    assertDependencies(_notAssociativeNorCommutative, BSON_ARRAY("ab.c"));
}