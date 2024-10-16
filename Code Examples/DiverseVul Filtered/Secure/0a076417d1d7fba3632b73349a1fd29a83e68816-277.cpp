static Value serializeConstant(Value val) {
    if (val.missing()) {
        return Value("$$REMOVE"_sd);
    }

    return Value(DOC("$const" << val));
}