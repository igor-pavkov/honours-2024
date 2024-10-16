TEST_F(ExpressionNaryTest, AddedConstantOperandIsSerialized) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    _notAssociativeNorCommutative->addOperand(ExpressionConstant::create(expCtx, Value(9)));
    assertContents(_notAssociativeNorCommutative, BSON_ARRAY(9));
}