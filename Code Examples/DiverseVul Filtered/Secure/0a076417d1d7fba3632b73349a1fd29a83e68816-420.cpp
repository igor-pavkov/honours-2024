    BSONObj spec() {
        return BSON("$gte" << BSON_ARRAY(1 << 0));
    }