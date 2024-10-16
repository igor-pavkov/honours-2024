Value ExpressionMeta::serialize(bool explain) const {
    switch (_metaType) {
        case MetaType::TEXT_SCORE:
            return Value(DOC("$meta"
                             << "textScore"_sd));
        case MetaType::RAND_VAL:
            return Value(DOC("$meta"
                             << "randVal"_sd));
    }
    MONGO_UNREACHABLE;
}