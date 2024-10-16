    virtual Value evaluate(const Document& root) const {
        const Value lhs = vpOperand[0]->evaluate(root);

        uassert(17310,
                str::stream() << "both operands of $setIsSubset must be arrays. First "
                              << "argument is of type: "
                              << typeName(lhs.getType()),
                lhs.isArray());

        return setIsSubsetHelper(lhs.getArray(), _cachedRhsSet);
    }