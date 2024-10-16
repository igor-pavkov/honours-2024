TEST_F(ExpressionNaryTest, SerializationToBsonObj) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    _notAssociativeNorCommutative->addOperand(ExpressionConstant::create(expCtx, Value(5)));
    ASSERT_BSONOBJ_EQ(BSON("foo" << BSON("$testable" << BSON_ARRAY(BSON("$const" << 5)))),
                      BSON("foo" << _notAssociativeNorCommutative->serialize(false)));
}