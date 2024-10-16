TEST_F(ExpressionNaryTest, ValidateObjectExpressionDependency) {
    BSONObj spec = BSON("" << BSON("a"
                                   << "$x"
                                   << "q"
                                   << "$r"));
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    BSONElement specElement = spec.firstElement();
    VariablesParseState vps = expCtx->variablesParseState;
    _notAssociativeNorCommutative->addOperand(
        Expression::parseObject(expCtx, specElement.Obj(), vps));
    assertDependencies(_notAssociativeNorCommutative,
                       BSON_ARRAY("r"
                                  << "x"));
}