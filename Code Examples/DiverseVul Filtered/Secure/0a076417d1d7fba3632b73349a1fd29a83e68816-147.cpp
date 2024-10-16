TEST_F(ExpressionNaryTest, FlattenOptimizationNotDoneOnOtherExpressionsForAssociativeExpressions) {
    BSONArray spec = BSON_ARRAY(66 << "$path" << BSON("$sum" << BSON_ARRAY("$path" << 2)));
    addOperandArrayToExpr(_associativeOnly, spec);
    assertContents(_associativeOnly, spec);
    intrusive_ptr<Expression> optimized = _associativeOnly->optimize();
    ASSERT(_associativeOnly == optimized);
    assertContents(_associativeOnly, spec);
}