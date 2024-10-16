    BSONObj spec() {
        return BSON("$lt" << BSON_ARRAY(1 << "$a"));
    }