TEST_F(ExpressionNaryTest, InternalOperandOptimizationIsDone) {
    BSONArray spec = BSON_ARRAY(BSON("$and" << BSONArray()) << "$abc");
    addOperandArrayToExpr(_notAssociativeNorCommutative, spec);
    assertContents(_notAssociativeNorCommutative, spec);
    ASSERT(_notAssociativeNorCommutative == _notAssociativeNorCommutative->optimize());
    assertContents(_notAssociativeNorCommutative, BSON_ARRAY(true << "$abc"));
}