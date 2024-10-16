    BSONObj spec() {
        return BSON("$gt" << BSON_ARRAY(1 << 2));
    }