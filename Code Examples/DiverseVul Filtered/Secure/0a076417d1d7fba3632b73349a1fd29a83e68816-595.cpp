    BSONObj spec() {
        return BSON("$cmp" << BSON_ARRAY(1 << "$a"));
    }