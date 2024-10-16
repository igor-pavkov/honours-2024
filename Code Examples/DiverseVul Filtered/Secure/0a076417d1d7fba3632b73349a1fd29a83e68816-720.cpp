ExpressionMap::ExpressionMap(const boost::intrusive_ptr<ExpressionContext>& expCtx,
                             const string& varName,
                             Variables::Id varId,
                             intrusive_ptr<Expression> input,
                             intrusive_ptr<Expression> each)
    : Expression(expCtx), _varName(varName), _varId(varId), _input(input), _each(each) {}