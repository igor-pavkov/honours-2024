TEST_F(ExpressionNaryTest, FlattenInnerOperandsOptimizationOnNotCommutativeNorAssociative) {
    BSONArrayBuilder specBuilder;

    intrusive_ptr<Testable> innerOperand = Testable::create(false, false);
    addOperandArrayToExpr(innerOperand, BSON_ARRAY(100 << "$path1"));
    specBuilder.append(expressionToBson(innerOperand));
    _notAssociativeNorCommutative->addOperand(innerOperand);

    addOperandArrayToExpr(_notAssociativeNorCommutative, BSON_ARRAY(99 << "$path2"));
    specBuilder << 99 << "$path2";

    BSONArray spec = specBuilder.arr();
    assertContents(_notAssociativeNorCommutative, spec);
    intrusive_ptr<Expression> optimized = _notAssociativeNorCommutative->optimize();
    ASSERT(_notAssociativeNorCommutative == optimized);

    assertContents(_notAssociativeNorCommutative, spec);
}