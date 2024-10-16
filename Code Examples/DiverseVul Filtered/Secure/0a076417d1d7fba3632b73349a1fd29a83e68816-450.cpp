TEST_F(ExpressionNaryTest, GroupingOptimizationOnAssociativeOnlyBackOperands) {
    BSONArray spec = BSON_ARRAY("$path" << 55 << 66);
    addOperandArrayToExpr(_associativeOnly, spec);
    assertContents(_associativeOnly, spec);
    intrusive_ptr<Expression> optimized = _associativeOnly->optimize();
    ASSERT(_associativeOnly == optimized);
    assertContents(_associativeOnly, BSON_ARRAY("$path" << BSON_ARRAY(55 << 66)));
}