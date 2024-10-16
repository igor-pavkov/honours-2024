TEST_F(ExpressionNaryTest, FlattenInnerOperandsOptimizationOnCommutativeAndAssociative) {
    BSONArrayBuilder specBuilder;

    addOperandArrayToExpr(_associativeAndCommutative, BSON_ARRAY(200 << "$path3" << 201));
    specBuilder << 200 << "$path3" << 201;

    intrusive_ptr<Testable> innerOperand = Testable::create(true, true);
    addOperandArrayToExpr(innerOperand, BSON_ARRAY(100 << "$path1" << 101));
    specBuilder.append(expressionToBson(innerOperand));
    _associativeAndCommutative->addOperand(innerOperand);

    addOperandArrayToExpr(_associativeAndCommutative, BSON_ARRAY(99 << "$path2"));
    specBuilder << 99 << "$path2";

    BSONArray spec = specBuilder.arr();
    assertContents(_associativeAndCommutative, spec);
    intrusive_ptr<Expression> optimized = _associativeAndCommutative->optimize();
    ASSERT(_associativeAndCommutative == optimized);

    BSONArray expectedContent = BSON_ARRAY("$path3"
                                           << "$path1"
                                           << "$path2"
                                           << BSON_ARRAY(200 << 201 << BSON_ARRAY(100 << 101)
                                                             << 99));
    assertContents(_associativeAndCommutative, expectedContent);
}