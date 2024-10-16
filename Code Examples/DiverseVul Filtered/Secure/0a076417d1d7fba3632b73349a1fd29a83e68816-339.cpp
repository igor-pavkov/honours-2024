    BSONObj spec() {
        return BSON("$or" << BSON_ARRAY(1 << "$a"));
    }