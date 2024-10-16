    virtual void assertEvaluates(Value input, Value output) {
        addOperand(_expr, input);
        ASSERT_VALUE_EQ(output, _expr->evaluate(Document()));
        ASSERT_EQUALS(output.getType(), _expr->evaluate(Document()).getType());
    }