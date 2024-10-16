TEST_F(ExpressionNaryTest, FlattenInnerOperandsOptimizationOnAssociativeOnlyMiddleOperandAndGroup) {
    BSONArrayBuilder specBuilder;

    addOperandArrayToExpr(_associativeOnly, BSON_ARRAY(200 << "$path3" << 201));
    specBuilder << 200 << "$path3" << 201;

    intrusive_ptr<Testable> innerOperand = Testable::create(true, false);
    addOperandArrayToExpr(innerOperand, BSON_ARRAY(100 << "$path1" << 101));
    specBuilder.append(expressionToBson(innerOperand));
    _associativeOnly->addOperand(innerOperand);

    addOperandArrayToExpr(_associativeOnly, BSON_ARRAY(99 << "$path2"));
    specBuilder << 99 << "$path2";

    BSONArray spec = specBuilder.arr();
    assertContents(_associativeOnly, spec);
    intrusive_ptr<Expression> optimized = _associativeOnly->optimize();
    ASSERT(_associativeOnly == optimized);

    BSONArray expectedContent = BSON_ARRAY(
        200 << "$path3" << BSON_ARRAY(201 << 100) << "$path1" << BSON_ARRAY(101 << 99) << "$path2");
    assertContents(_associativeOnly, expectedContent);
}