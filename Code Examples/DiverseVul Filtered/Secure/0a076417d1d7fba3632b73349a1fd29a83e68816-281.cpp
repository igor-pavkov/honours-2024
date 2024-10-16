TEST_F(ExpressionNaryTest, FlattenOptimizationNotDoneOnSameButNotAssociativeExpression) {
    BSONArrayBuilder specBuilder;

    intrusive_ptr<Testable> innerOperand = Testable::create(false, false);
    addOperandArrayToExpr(innerOperand, BSON_ARRAY(100 << "$path1" << 101));
    specBuilder.append(expressionToBson(innerOperand));
    _associativeOnly->addOperand(innerOperand);

    addOperandArrayToExpr(_associativeOnly, BSON_ARRAY(99 << "$path2"));
    specBuilder << 99 << "$path2";

    BSONArray spec = specBuilder.arr();
    assertContents(_associativeOnly, spec);
    intrusive_ptr<Expression> optimized = _associativeOnly->optimize();
    ASSERT(_associativeOnly == optimized);

    assertContents(_associativeOnly, spec);
}