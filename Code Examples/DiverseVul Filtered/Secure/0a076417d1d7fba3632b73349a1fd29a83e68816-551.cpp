intrusive_ptr<ExpressionConstant> makeConstant(T&& val) {
    intrusive_ptr<ExpressionContextForTest> expCtx(new ExpressionContextForTest());
    return ExpressionConstant::create(expCtx, Value(std::forward<T>(val)));
}