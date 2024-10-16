    Optimized(const boost::intrusive_ptr<ExpressionContext>& expCtx,
              const ValueSet& cachedRhsSet,
              const ExpressionVector& operands)
        : ExpressionSetIsSubset(expCtx), _cachedRhsSet(cachedRhsSet) {
        vpOperand = operands;
    }