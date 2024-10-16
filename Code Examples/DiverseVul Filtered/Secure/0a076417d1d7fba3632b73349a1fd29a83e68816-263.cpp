    BSONObj spec() {
        return BSON("$and" << BSON_ARRAY(1 << BSON("$and" << BSON_ARRAY(1)) << "$a"
                                           << "$b"));
    }