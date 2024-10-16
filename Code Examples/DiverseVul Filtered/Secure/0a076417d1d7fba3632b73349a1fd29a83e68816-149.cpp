intrusive_ptr<Expression> ExpressionObject::optimize() {
    for (auto&& pair : _expressions) {
        pair.second = pair.second->optimize();
    }
    return this;
}