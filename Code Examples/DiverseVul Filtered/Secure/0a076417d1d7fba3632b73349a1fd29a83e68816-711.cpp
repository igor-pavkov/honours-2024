TEST_F(ExpressionNaryTest, SerializationToBsonArr) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    _notAssociativeNorCommutative->addOperand(ExpressionConstant::create(expCtx, Value(5)));
    ASSERT_BSONOBJ_EQ(constify(BSON_ARRAY(BSON("$testable" << BSON_ARRAY(5)))),
                      BSON_ARRAY(_notAssociativeNorCommutative->serialize(false)));
}