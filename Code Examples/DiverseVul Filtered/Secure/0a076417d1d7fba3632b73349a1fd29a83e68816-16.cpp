TEST_F(ExpressionNaryTest, AddedFieldPathOperandIsSerialized) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    _notAssociativeNorCommutative->addOperand(ExpressionFieldPath::create(expCtx, "ab.c"));
    assertContents(_notAssociativeNorCommutative, BSON_ARRAY("$ab.c"));
}