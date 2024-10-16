static void assertExpectedResults(
    const string& expression,
    initializer_list<pair<vector<ImplicitValue>, ImplicitValue>> operations) {
    for (auto&& op : operations) {
        try {
            Value result = evaluateExpression(expression, op.first);
            ASSERT_VALUE_EQ(op.second, result);
            ASSERT_EQUALS(op.second.getType(), result.getType());
        } catch (...) {
            log() << "failed with arguments: " << ImplicitValue::convertToValue(op.first);
            throw;
        }
    }
}