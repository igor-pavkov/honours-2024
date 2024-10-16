    BSONObj spec() {
        return BSON("$and" << BSON_ARRAY(0 << "$a"));
    }