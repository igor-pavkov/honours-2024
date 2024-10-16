TEST_F(ExpressionNaryTest, AllConstantOperandOptimization) {
    BSONArray spec = BSON_ARRAY(1 << 2);
    addOperandArrayToExpr(_notAssociativeNorCommutative, spec);
    assertContents(_notAssociativeNorCommutative, spec);
    intrusive_ptr<Expression> optimized = _notAssociativeNorCommutative->optimize();
    ASSERT(_notAssociativeNorCommutative != optimized);
    ASSERT_BSONOBJ_EQ(BSON("$const" << BSON_ARRAY(1 << 2)), expressionToBson(optimized));
}