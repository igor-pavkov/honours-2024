ExpressionMeta::ExpressionMeta(const boost::intrusive_ptr<ExpressionContext>& expCtx,
                               MetaType metaType)
    : Expression(expCtx), _metaType(metaType) {}