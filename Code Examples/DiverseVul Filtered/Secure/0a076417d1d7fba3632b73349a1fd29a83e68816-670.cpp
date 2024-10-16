    Testable(bool isAssociative, bool isCommutative)
        : ExpressionNary(
              boost::intrusive_ptr<ExpressionContextForTest>(new ExpressionContextForTest())),
          _isAssociative(isAssociative),
          _isCommutative(isCommutative) {}