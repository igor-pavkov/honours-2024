TEST_F(ExpressionNaryTest, ValidateConstantExpressionDependency) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    _notAssociativeNorCommutative->addOperand(ExpressionConstant::create(expCtx, Value(1)));
    assertDependencies(_notAssociativeNorCommutative, BSONArray());
}