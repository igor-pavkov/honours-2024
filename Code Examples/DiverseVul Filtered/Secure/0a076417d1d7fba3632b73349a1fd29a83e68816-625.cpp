    BSONObj spec() {
        return BSON("$eq" << BSON_ARRAY(1 << 1));
    }