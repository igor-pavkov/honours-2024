boost::intrusive_ptr<Expression> ExpressionZip::optimize() {
    std::transform(_inputs.begin(),
                   _inputs.end(),
                   _inputs.begin(),
                   [](intrusive_ptr<Expression> inputExpression) -> intrusive_ptr<Expression> {
                       return inputExpression->optimize();
                   });

    std::transform(_defaults.begin(),
                   _defaults.end(),
                   _defaults.begin(),
                   [](intrusive_ptr<Expression> defaultExpression) -> intrusive_ptr<Expression> {
                       return defaultExpression->optimize();
                   });

    return this;
}