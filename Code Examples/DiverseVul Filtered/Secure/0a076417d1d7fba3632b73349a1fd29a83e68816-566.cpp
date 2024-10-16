Value ExpressionNary::serialize(bool explain) const {
    const size_t nOperand = vpOperand.size();
    vector<Value> array;
    /* build up the array */
    for (size_t i = 0; i < nOperand; i++)
        array.push_back(vpOperand[i]->serialize(explain));

    return Value(DOC(getOpName() << array));
}