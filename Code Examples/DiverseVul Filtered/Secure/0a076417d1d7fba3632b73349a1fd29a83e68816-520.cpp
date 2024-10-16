    BSONObj spec() {
        return BSON("$and" << BSON_ARRAY(0 << 1 << "$a"));
    }