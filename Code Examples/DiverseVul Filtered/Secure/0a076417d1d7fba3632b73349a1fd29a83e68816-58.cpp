void ExpressionNary::addOperand(const intrusive_ptr<Expression>& pExpression) {
    vpOperand.push_back(pExpression);
}