    void appendInvalidStringElement(const char* fieldName, BufBuilder* bb) {
        // like a BSONObj string, but without a NUL terminator.
        bb->appendChar(String);
        bb->appendStr(fieldName, /*withNUL*/true);
        bb->appendNum(4);
        bb->appendStr("asdf", /*withNUL*/false);
    }