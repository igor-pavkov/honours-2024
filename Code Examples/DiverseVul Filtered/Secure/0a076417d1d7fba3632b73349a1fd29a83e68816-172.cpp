TEST_F(ExpressionNaryTest, GroupingOptimizationOnAssociativeOnlyNotExecuteOnSingleConstantsMiddle) {
    BSONArray spec = BSON_ARRAY("$path1" << 55 << "$path2");
    addOperandArrayToExpr(_associativeOnly, spec);
    assertContents(_associativeOnly, spec);
    intrusive_ptr<Expression> optimized = _associativeOnly->optimize();
    ASSERT(_associativeOnly == optimized);
    assertContents(_associativeOnly, BSON_ARRAY("$path1" << 55 << "$path2"));
}