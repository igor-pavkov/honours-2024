Value ExpressionMeta::evaluate(const Document& root) const {
    switch (_metaType) {
        case MetaType::TEXT_SCORE:
            return root.hasTextScore() ? Value(root.getTextScore()) : Value();
        case MetaType::RAND_VAL:
            return root.hasRandMetaField() ? Value(root.getRandMetaField()) : Value();
    }
    MONGO_UNREACHABLE;
}